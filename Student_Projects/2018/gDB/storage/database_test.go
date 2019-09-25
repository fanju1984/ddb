package storage

import (
	"github.com/juju/errors"
	"github.com/sirupsen/logrus"
	"testing"
)

/*
	Add your own paths and ports to test the database.go.
	But please do not commit your configuration with git.
*/

func TestStartDatabase(t *testing.T) {
	db := Database{}
	db.PGBinPath = "/Library/PostgreSQL/11/bin/"
	db.PGDataPath = "/Users/miaonot/Documents/running_test/test1"
	db.PGPort = 5434
	db.StartDatabase()
}

func TestStopDatabase(t *testing.T) {
	db := Database{}
	db.PGBinPath = "/Library/PostgreSQL/11/bin/"
	db.PGDataPath = "/Users/miaonot/Documents/running_test/test1"
	db.StopDatabase()
}

func TestIsDatabaseAlive(t *testing.T) {
	db := Database{}
	db.PGBinPath = "/Library/PostgreSQL/11/bin/"
	db.PGDataPath = "/Users/miaonot/Documents/running_test/test_"
	db.IsDatabaseAlive()
}

func TestInitDatabase(t *testing.T) {
	db := Database{}
	db.PGBinPath = ""
	db.PGDataPath = ""
	db.initDatabase()
}

func TestDatabase_Connect(t *testing.T) {
	db := Database{}
	db.PGBinPath = "/Library/PostgreSQL/11/bin/"
	db.PGDataPath = "/Users/miaonot/Documents/running_test/test"
	db.PGPort = 5433
	db.StartDatabase()
	if _, err := db.Connect("postgres"); err != nil {
		logrus.Errorf("error: %s", errors.Trace(err))
	}
	db.StopDatabase()
}
