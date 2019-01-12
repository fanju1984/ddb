package server

import (
	"context"
	"fmt"
	"gDB/dataManager"
	"gDB/gddGet"
	pb "gDB/pb"
	"gDB/storage"
	"github.com/coreos/etcd/clientv3"
	"github.com/coreos/etcd/embed"
	"github.com/coreos/etcd/etcdserver/membership"
	"github.com/coreos/pkg/capnslog"
	"github.com/juju/errors"
	log "github.com/sirupsen/logrus"
	"google.golang.org/grpc"
	"net"
	"path"
	"strconv"
	"strings"
	"sync"
	"sync/atomic"
	"time"
)

const (
	etcdTimeout   = time.Second * 3
	stRootPath    = "/st"
	LeaderPath    = "/st/leader"
	clusterIDPath = "/st/cluster_id"

	// DefaultRequestTimeout 10s is long enough for most of etcd clusters.
	DefaultRequestTimeout = 10 * time.Second
	// DefaultSlowRequestTime 1s for the threshold for normal request, for those
	// longer then 1s, they are considered as slow requests.
	DefaultSlowRequestTime = 1 * time.Second
)

type Server struct {
	// Server state.
	isServing int64
	isLeader  int64

	// Configs and initial fields.
	cfg     *Config
	etcdCfg *embed.Config

	leaderLoopCtx    context.Context
	leaderLoopCancel func()
	leaderLoopWg     sync.WaitGroup

	// Etcd and cluster informations.
	etcd        *embed.Etcd
	client      *clientv3.Client
	id          uint64 // etcd server id.
	leaderValue string

	rpcServer *grpc.Server
}

//TODO: Not sure if handler is needed, test
func CreateServer(cfg *Config) (*Server, error) {
	s := &Server{
		cfg: cfg,
	}

	// Adjust etcd config.
	etcdCfg, err := s.cfg.genEmbedEtcdConfig()
	if err != nil {
		return nil, errors.Trace(err)
	}
	s.etcdCfg = etcdCfg

	//adjust log level
	level, err := log.ParseLevel(cfg.LogLevel)
	if err != nil {
		level = 1
	}
	log.SetLevel(level)
	capnslog.SetGlobalLogLevel(capnslog.WARNING)

	// Copy database config from cfg to db
	storage.InitDB(cfg.PGPort, s.cfg.PGUsername, s.cfg.PGPassword, s.cfg.PGBinPath, s.cfg.PGDataPath)

	dataManager.Init(s.cfg.Name)

	return s, nil
}

func (s *Server) Run(ctx context.Context) error {
	if err := s.startEtcd(ctx); err != nil {
		return errors.Trace(err)
	}

	if err := s.startServer(); err != nil {
		return errors.Trace(err)
	}

	//s.startLeaderLoop()

	if err := storage.StartDB(); err != nil {
		return errors.Trace(err)
	}

	return nil
}

func (s *Server) startEtcd(ctx context.Context) error {
	log.Info("start embed etcd")
	etcd, err := embed.StartEtcd(s.etcdCfg)
	if err != nil {
		return errors.Trace(err)
	}

	//因为是实验用，默认配置正常，省略了cluster的check

	select {
	// Wait etcd until it is ready to use
	case <-etcd.Server.ReadyNotify():
	case <-ctx.Done():
		return errors.Errorf("canceled when waiting embed etcd to be ready")
	}

	endpoints := []string{s.etcdCfg.ACUrls[0].String()}
	log.Infof("create etcd v3 client with endpoints %v", endpoints)

	client, err := clientv3.New(clientv3.Config{
		Endpoints:   endpoints,
		DialTimeout: etcdTimeout,
		TLS:         nil,
	})
	if err != nil {
		return errors.Trace(err)
	}

	etcdServerID := uint64(etcd.Server.ID())

	// update advertise peer urls.
	etcdMembers, err := listEtcdMembers(client)
	if err != nil {
		return errors.Trace(err)
	}
	for _, m := range etcdMembers.Members {
		if etcdServerID == m.ID {
			etcdPeerURLs := strings.Join(m.PeerURLs, ",")
			if s.cfg.AdvertisePeerUrls != etcdPeerURLs {
				log.Infof("update advertise peer urls from %s to %s", s.cfg.AdvertisePeerUrls, etcdPeerURLs)
				s.cfg.AdvertisePeerUrls = etcdPeerURLs
			}
		}
	}

	s.etcd = etcd
	s.client = client
	s.id = etcdServerID

	gddGet.Init(s.client)
	return nil
}

// listEtcdMembers returns a list of internal etcd members.
func listEtcdMembers(client *clientv3.Client) (*clientv3.MemberListResponse, error) {
	ctx, cancel := context.WithTimeout(client.Ctx(), DefaultRequestTimeout)
	listResp, err := client.MemberList(ctx)
	cancel()
	return listResp, errors.Trace(err)
}

func (s *Server) startServer() error {
	id := strconv.FormatUint(s.id, 10)
	name := s.cfg.Name
	clientUrls := s.cfg.AdvertiseClientUrls
	peerUrls := s.cfg.AdvertisePeerUrls
	s.leaderValue = id + "-" + name + "-" + clientUrls + "-" + peerUrls

	// Server has started.
	atomic.StoreInt64(&s.isServing, 1)

	return nil
}

// Close closes the server.
func (s *Server) Close() {
	if !atomic.CompareAndSwapInt64(&s.isServing, 1, 0) {
		// server is already closed
		return
	}

	log.Info("closing server")

	//s.stopLeaderLoop()

	if s.client != nil {
		s.client.Close()
	}

	if s.etcd != nil {
		s.etcd.Close()
	}

	storage.StopDB()

	log.Info("close server")
}

func (s *Server) stopLeaderLoop() {
	s.leaderLoopCancel()
	s.leaderLoopWg.Wait()
}

func (s *Server) isClosed() bool {
	return atomic.LoadInt64(&s.isServing) == 0
}

func (s *Server) getMemberLeaderPriorityPath(id uint64) string {
	return path.Join(stRootPath, fmt.Sprintf("member/%d/leader_priority", id))
}

func (s *Server) StartRPC() {
	s.startRPCClient()

	lis, err := net.Listen("tcp", s.cfg.GRPCUrls)
	if err != nil {
		log.Fatalf("tcp listen port failed: %v", errors.ErrorStack(err))
	}
	s.rpcServer = grpc.NewServer(grpc.MaxRecvMsgSize(64*1024*1024), grpc.MaxSendMsgSize(64*1024*1024))
	pb.RegisterPeerCommunicationServer(s.rpcServer, s)
	log.Infof("gRPC is ready to serve at %v", s.cfg.GRPCUrls)
	if err := s.rpcServer.Serve(lis); err != nil {
		log.Fatalf("run rpc failed: %v", errors.ErrorStack(err))
	}
}

func (s *Server) startRPCClient() {
	storage.InitRPCClient(s.cfg.GRPCPeers)
}

func (s *Server) StopRPC() {
	s.rpcServer.Stop()
	log.Infof("close gRPC")

	storage.CloseRPCClient()
}

func (s *Server) AddEtcd(name string, peerUrls string) error {
	now := time.Now()
	url, err := parseUrls(peerUrls)
	if err != nil {
		return errors.Trace(err)
	}
	s.etcd.Server.AddMember(context.Background(), *membership.NewMember(name, url, "", &now))
	return nil
}
