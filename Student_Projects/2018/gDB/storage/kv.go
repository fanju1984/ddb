package storage

import (
	"context"
	"github.com/coreos/etcd/clientv3"
	"github.com/juju/errors"
	log "github.com/sirupsen/logrus"
	"time"
)

const (
	kvRequestTimeout  = time.Second * 10
	kvSlowRequestTime = time.Second * 1

	rootPath = "/gDB"
)

// TODO: 返回etcd路径，如下所示，需要知道需要返回哪些路径
//func getFirstClassPath(p string) string {
//	return path.Join(rootPath, p)
//}

func KvGet(c *clientv3.Client, key string, opts ...clientv3.OpOption) (*clientv3.GetResponse, error) {
	ctx, cancel := context.WithTimeout(c.Ctx(), kvRequestTimeout)
	defer cancel()

	start := time.Now()
	resp, err := clientv3.NewKV(c).Get(ctx, key, opts...)
	if err != nil {
		log.Errorf("load from etcd error: %v", err)
	}
	if cost := time.Since(start); cost > kvSlowRequestTime {
		log.Warnf("kv gets too slow: key %v cost %v err %v", key, cost, err)
	}

	return resp, errors.Trace(err)
}

// A helper function to get value with key from etcd.
// TODO: return the value revision for outer use.
func GetValue(c *clientv3.Client, key string, opts ...clientv3.OpOption) ([]byte, error) {
	resp, err := KvGet(c, key, opts...)
	if err != nil {
		return nil, errors.Trace(err)
	}

	if n := len(resp.Kvs); n == 0 {
		return nil, nil
	} else if n > 1 {
		return nil, errors.Errorf("invalid get value resp %v, must only one", resp.Kvs)
	}

	return resp.Kvs[0].Value, nil
}

func PutValue(c *clientv3.Client, key string, val string, opts ...clientv3.OpOption) error {
	ctx, cancel := context.WithTimeout(c.Ctx(), kvRequestTimeout)
	defer cancel()

	start := time.Now()
	_, err := clientv3.NewKV(c).Put(ctx, key, val, opts...)
	if err != nil {
		log.Errorf("load from etcd error: %v", err)
	}
	if cost := time.Since(start); cost > kvSlowRequestTime {
		log.Warnf("kv puts too slow: key %v cost %v err %v", key, cost, err)
	}

	return errors.Trace(err)
}

func DeleteValue(c *clientv3.Client, key string, opts ...clientv3.OpOption) (int64, error) {
	ctx, cancel := context.WithTimeout(c.Ctx(), kvRequestTimeout)
	defer cancel()

	start := time.Now()
	response, err := clientv3.NewKV(c).Delete(ctx, key, opts...)
	if err != nil {
		log.Errorf("load from etcd error: %v", err)
	}
	if cost := time.Since(start); cost > kvSlowRequestTime {
		log.Warnf("kv deletes too slow: key %v cost %v err %v", key, cost, err)
	}

	return response.Deleted, errors.Trace(err)
}
