# 把该文件放在/mnt/d/wsl/s2目录下，并执行：
# 1，rm -r data.etcd/
# 2，sh sets2.sh
TOKEN=token-03
CLUSTER_STATE=new
NAME_1=s1
NAME_2=s2
NAME_3=s3
NAME_4=s4
HOST_1=10.46.179.127
HOST_2=10.46.219.70
HOST_3=10.46.33.43
HOST_4=10.46.33.43
CLUSTER=${NAME_1}=http://${HOST_1}:2380,${NAME_2}=http://${HOST_2}:2380,${NAME_3}=http://${HOST_3}:2380,${NAME_4}=http://${HOST_4}:22380

# For machine 2
THIS_NAME=${NAME_2}
THIS_IP=${HOST_2}
etcd --data-dir=data.etcd --name ${THIS_NAME} \
    --initial-advertise-peer-urls http://${THIS_IP}:2380 --listen-peer-urls http://${THIS_IP}:2380 \
    --advertise-client-urls http://${THIS_IP}:2379 --listen-client-urls http://${THIS_IP}:2379 \
    --initial-cluster ${CLUSTER} \
    --initial-cluster-state ${CLUSTER_STATE} --initial-cluster-token ${TOKEN}
