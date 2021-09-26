# 下载安装etcd
ETCD_VER=v3.4.2
GITHUB_URL=https://github.com/etcd-io/etcd/releases/download
DOWNLOAD_URL=${GITHUB_URL}

wget ${DOWNLOAD_URL}/${ETCD_VER}/etcd-${ETCD_VER}-linux-amd64.tar.gz
tar -zxvf ./etcd-v3.4.2-linux-amd64.tar.gz
mkdir /usr/local/etcd
cp ./etcd-v3.4.2-linux-amd64/etcd* /usr/local/etcd

# 设置环境变量
sudo vi /etc/profile
export PATH=$PATH:/usr/local/etcd
source /etc/profile

# curl依赖包
sudo apt-get install curl
sudo apt-get install libcurl4-openssl-dev

# json依赖包
参考 https://blog.csdn.net/wenqiang1208/article/details/75578383
# 注意此处需要使用python2
apt-get install python2
whereis python2
sudo ln -s /usr/bin/python2 /usr/bin/python

