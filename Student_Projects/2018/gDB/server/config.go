package server

import (
	"flag"
	"fmt"
	"github.com/BurntSushi/toml"
	"github.com/coreos/etcd/embed"
	"github.com/juju/errors"
	"net/url"
	"path/filepath"
	"strings"
	"time"
)

type Config struct {
	*flag.FlagSet `json:"-"`

	configFile string

	ClientUrls          string `toml:"client-urls" json:"client-urls"`
	PeerUrls            string `toml:"peer-urls" json:"peer-urls"`
	AdvertiseClientUrls string `toml:"advertise-client-urls" json:"advertise-client-urls"`
	AdvertisePeerUrls   string `toml:"advertise-peer-urls" json:"advertise-peer-urls"`

	Name    string `toml:"name" json:"name"`
	DataDir string `toml:"data-dir" json:"data-dir"`

	InitialCluster      string `toml:"initial-cluster" json:"initial-cluster"`
	InitialClusterState string `toml:"initial-cluster-state" json:"initial-cluster-state"`

	// AutoCompactionMode is either 'periodic' or 'revision'. The default value is 'periodic'.
	AutoCompactionMode string `toml:"auto-compaction-mode" json:"auto-compaction-mode"`
	// AutoCompactionRetention is either duration string with time unit
	// (e.g. '5m' for 5-minute), or revision unit (e.g. '5000').
	// If no time unit is provided and compaction mode is 'periodic',
	// the unit defaults to hour. For example, '5' translates into 5-hour.
	// The default retention is 1 hour.
	// Before etcd v3.3.x, the type of retention is int. We add 'v2' suffix to make it backward compatible.
	AutoCompactionRetention string `toml:"auto-compaction-retention" json:"auto-compaction-retention-v2"`

	//Added values
	//Setting gRPC
	GRPCUrls  string `toml:"grpc-urls" json:"grpc-urls"`
	GRPCPeers string `toml:"grpc-peers" json:"grpc-peers"`

	//Setting PG connection
	PGUsername string `toml:"pg-username" json:"pg-username"`
	PGPassword string `toml:"pg-password" json:"pg-password"`
	PGBinPath  string `toml:"pg-bin-path" json:"pg-bin-path"`
	PGDataPath string `toml:"pg-data-path" json:"pg-data-path"`
	PGPort     int    `toml:"pg-port" json:"pg-port"`

	LogLevel string `toml:"log-level" json:"log-level"`

	//Below are all using default value

	// LeaderLease time, if leader doesn't update its TTL
	// in etcd after lease time, etcd will expire the leader key
	// and other servers can campaign the leader again.
	// Etcd onlys support seoncds TTL, so here is second too.
	LeaderLease int64 `toml:"lease" json:"lease"`

	// TsoSaveInterval is the interval to save timestamp.
	//TsoSaveInterval Duration `toml:"tso-save-interval" json:"tso-save-interval"`

	// TickInterval is the interval for etcd Raft tick.
	TickInterval Duration `toml:"tick-interval"`
	// ElectionInterval is the interval for etcd Raft election.
	ElectionInterval Duration `toml:"election-interval"`

	// QuotaBackendBytes Raise alarms when backend size exceeds the given quota. 0 means use the default quota.
	// the default size is 2GB, the maximum is 8GB.
	QuotaBackendBytes ByteSize `toml:"quota-backend-bytes" json:"quota-backend-bytes"`

	leaderPriorityCheckInterval Duration
}

// Duration is a wrapper of time.Duration for TOML and JSON.
type Duration struct {
	time.Duration
}

// ByteSize is a retype uint64 for TOML and JSON.
type ByteSize uint64

const (
	defaultName       = "gDB"
	defaultClientUrls = "http://127.0.0.1:2379"
	defaultPeerUrls   = "http://127.0.0.1:2380"

	defaultLeaderLease             = 3
	defaultCompactionMode          = "periodic"
	defaultAutoCompactionRetention = "1h"

	// etcd use 100ms for heartbeat and 1s for election timeout.
	// We can enlarge both a little to reduce the network aggression.
	// now embed etcd use TickMs for heartbeat, we will update
	// after embed etcd decouples tick and heartbeat.
	defaultTickInterval = 500 * time.Millisecond
	// embed etcd has a check that `5 * tick > election`
	defaultElectionInterval = 3000 * time.Millisecond

	defaultLeaderPriorityCheckInterval = time.Minute

	defaultGRPCUrls = "localhost:50051"

	defaultLogLevel = "warning"
)

func NewConfig() *Config {
	cfg := Config{}

	cfg.FlagSet = flag.NewFlagSet("gDB", flag.ContinueOnError)

	cfg.FlagSet.StringVar(&cfg.configFile, "config", "", "Config file")

	cfg.FlagSet.StringVar(&cfg.Name, "name", defaultName, "human-readable name for this pd member")

	cfg.FlagSet.StringVar(&cfg.ClientUrls, "client-urls", defaultClientUrls, "url for client traffic")
	cfg.FlagSet.StringVar(&cfg.AdvertiseClientUrls, "advertise-client-urls", "", "advertise url for client traffic (default '${client-urls}')")
	cfg.FlagSet.StringVar(&cfg.DataDir, "data-dir", "", "path to meta data directory (default 'default.${name}')")
	cfg.FlagSet.StringVar(&cfg.PeerUrls, "peer-urls", defaultPeerUrls, "url for peer traffic")
	cfg.FlagSet.StringVar(&cfg.AdvertisePeerUrls, "advertise-peer-urls", "", "advertise url for peer traffic (default '${peer-urls}')")
	cfg.FlagSet.StringVar(&cfg.InitialCluster, "initial-cluster", "", "initial cluster configuration for bootstrapping, e,g. pd=http://127.0.0.1:2380")

	cfg.FlagSet.StringVar(&cfg.GRPCUrls, "grpc-urls", defaultGRPCUrls, "url for gRPC traffic")
	cfg.FlagSet.StringVar(&cfg.GRPCPeers, "grpc-peers", "", "initial gRPC connection for bootstrapping, e,g. pd=http://127.0.0.1:50051")

	cfg.FlagSet.StringVar(&cfg.PGUsername, "pg-username", "", "username for connecting pg")
	cfg.FlagSet.StringVar(&cfg.PGPassword, "pg-password", "", "password for connecting pg")
	cfg.FlagSet.StringVar(&cfg.PGBinPath, "pg-bin-path", "", "bin path of pg")
	cfg.FlagSet.StringVar(&cfg.PGDataPath, "pg-data-path", "", "data path for pg")
	cfg.FlagSet.IntVar(&cfg.PGPort, "pg-port", 5433, "port for pg connection （default 5433)")

	cfg.FlagSet.StringVar(&cfg.LogLevel, "log-level", defaultLogLevel, "set log level, the valid value: panic, fatal, error, warning, info, debug, trace")

	return &cfg
}

func (c *Config) Parse(arguments []string) error {
	err := c.FlagSet.Parse(arguments)
	if err != nil {
		return errors.Trace(err)
	}

	if c.configFile != "" {
		_, err = toml.DecodeFile(c.configFile, c)
		if err != nil {
			return errors.Trace(err)
		}
	}

	// Parse again to replace with command line options.
	err = c.FlagSet.Parse(arguments)
	if err != nil {
		return errors.Trace(err)
	}

	if len(c.FlagSet.Args()) != 0 {
		return errors.Errorf("'%s' is an invalid flag", c.FlagSet.Arg(0))
	}

	err = c.adjust()
	return errors.Trace(err)
}

// configFromFile loads config from file.
func (c *Config) configFromFile(path string) (*toml.MetaData, error) {
	meta, err := toml.DecodeFile(path, c)
	return &meta, errors.Trace(err)
}

func (c *Config) adjust() error {
	adjustString(&c.DataDir, fmt.Sprintf("default.%s", c.Name))

	if err := c.validate(); err != nil {
		return errors.Trace(err)
	}

	adjustString(&c.AdvertiseClientUrls, c.ClientUrls)
	adjustString(&c.AdvertisePeerUrls, c.PeerUrls)

	if len(c.InitialCluster) == 0 {
		// The advertise peer urls may be http://127.0.0.1:2380,http://127.0.0.1:2381
		// so the initial cluster is pd=http://127.0.0.1:2380,pd=http://127.0.0.1:2381
		items := strings.Split(c.AdvertisePeerUrls, ",")

		sep := ""
		for _, item := range items {
			c.InitialCluster += fmt.Sprintf("%s%s=%s", sep, c.Name, item)
			sep = ","
		}
	}

	adjustInt64(&c.LeaderLease, defaultLeaderLease)
	//adjustDuration(&c.TsoSaveInterval, time.Duration(defaultLeaderLease)*time.Second)

	adjustString(&c.AutoCompactionMode, defaultCompactionMode)
	adjustString(&c.AutoCompactionRetention, defaultAutoCompactionRetention)
	adjustDuration(&c.TickInterval, defaultTickInterval)
	adjustDuration(&c.ElectionInterval, defaultElectionInterval)

	adjustDuration(&c.leaderPriorityCheckInterval, defaultLeaderPriorityCheckInterval)

	adjustString(&c.GRPCUrls, defaultGRPCUrls)
	adjustString(&c.GRPCPeers, defaultName+"="+c.GRPCUrls)

	adjustString(&c.LogLevel, defaultLogLevel)

	return nil
}

func adjustString(v *string, defValue string) {
	if len(*v) == 0 {
		*v = defValue
	}
}

func adjustInt64(v *int64, defValue int64) {
	if *v == 0 {
		*v = defValue
	}
}

func adjustDuration(v *Duration, defValue time.Duration) {
	if v.Duration == 0 {
		v.Duration = defValue
	}
}

func (c *Config) validate() error {
	_, err := filepath.Abs(c.DataDir)
	if err != nil {
		return errors.Trace(err)
	}
	return nil
}

func (c *Config) genEmbedEtcdConfig() (*embed.Config, error) {
	cfg := embed.NewConfig()
	cfg.Name = c.Name
	cfg.Dir = c.DataDir
	cfg.WalDir = ""
	cfg.InitialCluster = c.InitialCluster
	cfg.ClusterState = c.InitialClusterState
	cfg.EnablePprof = true
	cfg.StrictReconfigCheck = false
	cfg.TickMs = uint(c.TickInterval.Duration / time.Millisecond)
	cfg.ElectionMs = uint(c.ElectionInterval.Duration / time.Millisecond)
	cfg.AutoCompactionMode = c.AutoCompactionMode
	cfg.AutoCompactionRetention = c.AutoCompactionRetention
	cfg.QuotaBackendBytes = int64(c.QuotaBackendBytes)

	var err error

	cfg.LPUrls, err = parseUrls(c.PeerUrls)
	if err != nil {
		return nil, errors.Trace(err)
	}

	cfg.APUrls, err = parseUrls(c.AdvertisePeerUrls)
	if err != nil {
		return nil, errors.Trace(err)
	}

	cfg.LCUrls, err = parseUrls(c.ClientUrls)
	if err != nil {
		return nil, errors.Trace(err)
	}

	cfg.ACUrls, err = parseUrls(c.AdvertiseClientUrls)
	if err != nil {
		return nil, errors.Trace(err)
	}

	return cfg, nil
}

// parseUrls parse a string into multiple urls.
func parseUrls(s string) ([]url.URL, error) {
	items := strings.Split(s, ",")
	urls := make([]url.URL, 0, len(items))
	for _, item := range items {
		u, err := url.Parse(item)
		if err != nil {
			return nil, errors.Trace(err)
		}

		urls = append(urls, *u)
	}

	return urls, nil
}
