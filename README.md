# kube5GC

> This project is for deploying NCTU5GC on [kubeCORD](https://github.com/sufuf3/kubecord-installer).

## Table of Contents

- [NCTU-5GC Components](#nctu5gc-components)
- [Prepare](#prepare)
    - [1. Using kubeCORD deploy Kubernetes](#1-using-kubecord-deploy-kubernetes)
    - [2. Install NFS Server](#2-install-nfs-server)
    - [3. Install helm](#3-install-helm)
- [Deploy NCTU5GC](#deploy-nctu5gc)
    - [Method 1 - Using yaml](#method-1---using-yaml)
    - [Method 2 - Using Helm](#method-2---using-helm)

## NCTU-5GC Components

- 🚧 mongodb
    - pv, statefulset, service
- 🚧 webui
    - Docker: [nextepc-webui](https://cloud.docker.com/repository/docker/sufuf3/nextepc-webui)
    - service, deployment
- 🔨 nextepc-base
- 🔨 nextepc-build
- 🔨 mme
- 🚧 u1-u2
    - deployment
- 🔨 sgw
- 🔨 pgw
- 🔨 hss
- 🔨 pcrf

## Prepare

### 1. Using [kubeCORD](https://github.com/sufuf3/kubecord-installer) deploy Kubernetes

### 2. Install NFS Server

On node-1.

```sh
sudo apt-get install -qqy nfs-kernel-server
sudo mkdir -p /nfsshare/mongodb
echo "/nfsshare *(rw,sync,no_root_squash)" | sudo tee /etc/exports
sudo exportfs -r
sudo showmount -e
```

### 3. Install helm

On node-1.

```sh
curl -L https://storage.googleapis.com/kubernetes-helm/helm-v2.9.1-linux-amd64.tar.gz > helm-v2.9.1-linux-amd64.tar.gz && tar -zxvf helm-v2.9.1-linux-amd64.tar.gz && chmod +x linux-amd64/helm && sudo mv linux-amd64/helm /usr/local/bin/helm
rm -rf /home/$USER/helm-v2.9.1-linux-amd64.tar.gz
sudo pip install yq
helm init
kubectl create serviceaccount --namespace kube-system tiller
kubectl create clusterrolebinding tiller-cluster-rule --clusterrole=cluster-admin --serviceaccount=kube-system:tiller
kubectl patch deploy --namespace kube-system tiller-deploy -p '{"spec":{"template":{"spec":{"serviceAccount":"tiller"}}}}'
```

## Deploy NCTU5GC

### Method 1 - Using yaml

#### 1. Update `deploy/nctu5GC/mongodb/pv.yaml`

```sh
sed -i "s/192.168.26.11/${NFS_SERVER_IP}/g" deploy/nctu5GC/mongodb/pv.yaml
```

### Method 2 - Using Helm
