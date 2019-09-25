package storage

import (
	"database/sql"
	"fmt"
	"github.com/juju/errors"
	log "github.com/sirupsen/logrus"
	"io/ioutil"
	"os"
	"os/exec"
	"path"
	"strconv"
	"strings"

	_ "github.com/bmizerany/pq"
)

type Database struct {
	PGUsername      string
	PGPassword      string
	PGBinPath       string
	PGDataPath      string
	PGPort          int
	alternativeName string
}

// Use to start database. return error when the database can't be started.
// If the database cluster directory is not initial and can't be initialed, if will fatal immediately.
// If the port is in use, there is an error and no error message.
// Could PG make all this error return and error message consistent? So disgusting :(
func (db *Database) StartDatabase() error {
	if flag, _ := db.IsDatabaseAlive(); flag {
		log.Warnf("database has been started already.")
		return nil
	}
	startCmd := exec.Command(path.Join(db.PGBinPath, "/pg_ctl"), "-D", db.PGDataPath,
		"-o \"-p "+strconv.Itoa(db.PGPort)+"\"", "start")
	stderr, err := startCmd.StderrPipe()
	if err != nil {
		return errors.Trace(err)
	}
	if err := startCmd.Start(); err != nil {
		return errors.Trace(err)
	}
	slurp, _ := ioutil.ReadAll(stderr)
	log.Infof("%s", slurp)
	if err := startCmd.Wait(); err != nil {
		if strings.Count(string(slurp[:]), "is not a database cluster directory") == 1 ||
			strings.Count(string(slurp[:]), "does not exist") == 1 {
			err := db.initDatabase()
			if err != nil {
				log.Fatalf("Database cluster failed to initial with error: %s", errors.Trace(err))
			}
			startCmd := exec.Command(path.Join(db.PGBinPath, "/pg_ctl"), "-D", db.PGDataPath,
				"-o \"-p "+strconv.Itoa(db.PGPort)+"\"", "start")
			stderr, err := startCmd.StderrPipe()
			if err != nil {
				return errors.Trace(err)
			}
			startCmd.Start()
			slurp, _ = ioutil.ReadAll(stderr)
			if err := startCmd.Wait(); err != nil {
				return errors.Errorf(err.Error() + string(slurp[:]))
			} else if string(slurp[:]) == "" {
				if err := db.connectAndCreateTable(); err != nil {
					log.Fatalf("Database create failed, so we cannot move on. "+
						"Please create an database named the same as your system username in the directory manually."+
						"The failed reason is %s", err)
				}
				return nil
			} else {
				log.Infof(string(slurp[:]))
			}
		} else {
			return errors.Errorf(err.Error() + string(slurp[:]))
		}
	}
	return nil
}

// If the data path is not initial as database cluster directory, Initial it.
// There will be a error massage when successfully create database cluster directory, but no error returned. weird :(
func (db *Database) initDatabase() error {
	flag, err := db.dataPathExists()
	if err != nil {
		log.Warnf("Are we have the rights to access the data path? But we will still try it.")
	}
	if !flag {
		if err := os.Mkdir(db.PGDataPath, os.ModePerm); err != nil {
			log.Fatalf("There is no such data path and We can't make it. Run database failed.")
		}
	}
	log.Warnf("It seems you don't have a database in this directory, so we will create for you." +
		" You can delete it after close this program if you think that is a mistake.")
	initCmd := exec.Command(path.Join(db.PGBinPath, "/pg_ctl"), "-D", db.PGDataPath, "init")
	stderr, err := initCmd.StderrPipe()
	if err != nil {
		return errors.Trace(err)
	}
	if err := initCmd.Start(); err != nil {
		return errors.Trace(err)
	}
	slurp, _ := ioutil.ReadAll(stderr)
	log.Infof("%s", slurp)
	if err := initCmd.Wait(); err != nil {
		return errors.Errorf(err.Error() + string(slurp[:]))
	}
	if db.alternativeName, err = whoami(); err != nil {
		log.Error(errors.Trace(err))
		log.Fatalf("Database files create successfully, but we can't get your system username." +
			" You need to set it in your configuration file by yourself and restart.")
	}
	log.Warnf("Database user create successfully. Your username is %s, and no password.", db.alternativeName)
	db.PGUsername = db.alternativeName
	return nil
}

// Stop database
// There is a check before stop database. If error occurs, the stop steps will be skipped.
func (db *Database) StopDatabase() error {
	flag, err := db.IsDatabaseAlive()
	if err != nil {
		log.Infof("Something may wrong, skipped database close")
	}
	if flag {
		cmd := exec.Command(path.Join(db.PGBinPath, "/pg_ctl"), "-D", db.PGDataPath, "stop")
		stderr, err := cmd.StderrPipe()
		if err != nil {
			return errors.Trace(err)
		}
		if err := cmd.Start(); err != nil {
			return errors.Trace(err)
		}
		slurp, _ := ioutil.ReadAll(stderr)
		log.Infof("%s", slurp)
		if err := cmd.Wait(); err != nil {
			return errors.Trace(err)
		}
	}
	return nil
}

// Check if Database is still alive. True if alive, false if not.
// When there is an error occur, return error and bool should be ignored.
// In the "pg_ctl status", there is something strange:
// If the database is not running, it will get an error, but no error message.
// What's more, if the data path is not exist, there is even neither error nor error message.
func (db *Database) IsDatabaseAlive() (bool, error) {
	cmd := exec.Command(path.Join(db.PGBinPath, "/pg_ctl"), "-D", db.PGDataPath, "status")
	stderr, err := cmd.StdoutPipe()
	if err != nil {
		return false, errors.Trace(err)
	}
	if err := cmd.Start(); err != nil {
		return false, errors.Trace(err)
	}
	slurp, _ := ioutil.ReadAll(stderr)
	cmd.Wait()
	if string(slurp[:]) != "pg_ctl: no server running\n" && len(slurp) != 0 {
		log.Infof("'the database is alive' is %t", true)
		return true, nil
	}
	log.Infof("'the database is alive' is %t", false)
	return false, nil
}

func whoami() (string, error) {
	cmd := exec.Command("whoami")
	stdout, err := cmd.StdoutPipe()
	if err != nil {
		return "", errors.Trace(err)
	}
	if err := cmd.Start(); err != nil {
		return "", errors.Trace(err)
	}
	slurp, _ := ioutil.ReadAll(stdout)
	if err := cmd.Wait(); err != nil {
		return "", errors.Trace(err)
	}
	return strings.Replace(string(slurp[:]), "\n", "", -1), nil
}

func (db *Database) dataPathExists() (bool, error) {
	_, err := os.Stat(db.PGDataPath)
	if err == nil {
		return true, nil
	}
	if os.IsNotExist(err) {
		return false, nil
	}
	return false, err
}

func (db *Database) connectAndCreateTable() error {
	sqlInfo := fmt.Sprintf("host=%s port=%d user=%s password=%s dbname=%s sslmode=disable",
		"127.0.0.1", db.PGPort, db.PGUsername, db.PGPassword, "postgres")
	database, _ := sql.Open("postgres", sqlInfo)
	if _, err := database.Exec("CREATE DATABASE " + db.alternativeName); err != nil {
		return errors.Trace(err)
	}
	log.Warnf("create database successful")
	return nil
}

func (db *Database) Connect(databaseName string) (*sql.DB, error) {
	sqlInfo := fmt.Sprintf("host=%s port=%d user=%s password=%s dbname=%s sslmode=disable",
		"127.0.0.1", db.PGPort, db.PGUsername, db.PGPassword, databaseName)
	database, err := sql.Open("postgres", sqlInfo)
	if err != nil {
		return nil, errors.Trace(err)
	}
	err = database.Ping()
	if err != nil {
		if db.alternativeName != "" {
			return nil, errors.Trace(err)
		}
		log.Warnf("The username may be wrong, use system username to try.")
		if db.alternativeName, err = whoami(); err != nil {
			return nil, errors.Trace(err)
		}
		sqlInfo = fmt.Sprintf("host=%s port=%d user=%s password=%s dbname=%s sslmode=disable",
			"127.0.0.1", db.PGPort, db.alternativeName, db.PGPassword, databaseName)
		database, err := sql.Open("postgres", sqlInfo)
		if err != nil {
			return nil, errors.Trace(err)
		}
		err = database.Ping()
		if err != nil {
			log.Warnf("Maybe the password is also wrong, leave it blank and try.")
			db.PGPassword = ""
			sqlInfo = fmt.Sprintf("host=%s port=%d user=%s password=%s dbname=%s sslmode=disable",
				"127.0.0.1", db.PGPort, db.alternativeName, db.PGPassword, databaseName)
			database, err := sql.Open("postgres", sqlInfo)
			if err != nil {
				return nil, errors.Trace(err)
			}
			err = database.Ping()
			if err != nil {
				return nil, errors.Trace(err)
			}
		}
	}
	return database, nil
}
