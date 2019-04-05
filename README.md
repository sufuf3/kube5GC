# kube5GC

> This project is for deploying Free5GC on [kubeCORD](https://github.com/sufuf3/kubecord-installer).

## Table of Contents

- [Free5GC Components](#Free5gc-components)
- [Prepare](#prepare)
    - [1. Using kubeCORD deploy Kubernetes](#1-using-kubecord-deploy-kubernetes)
    - [2. Install NFS Server](#2-install-nfs-server)
    - [3. Install helm](#3-install-helm)
- [Deploy free5GC](#deploy-free5gc)
    - [Method 1 - Using yaml](#method-1---using-yaml)
    - [Method 2 - Using Helm](#method-2---using-helm)
- [Verification](#verification)

## Free5GC Components

### Components

- mongodb
- webui
- HSS
- PCRF
- AMF
- SMF
- UPF

### Docker images

- 🚧 webui
    - Docker: [nextepc-webui](https://cloud.docker.com/repository/docker/sufuf3/nextepc-webui)
- 🚧 nextepc-base
    - Docker: [nextepc-base](https://cloud.docker.com/repository/registry-1.docker.io/sufuf3/nextepc-base)
- 🚧 nextepc-build
    - Docker: [nextepc-build](https://cloud.docker.com/repository/registry-1.docker.io/sufuf3/nextepc-build)

## Prepare

### 1. Using [kubeCORD](https://github.com/sufuf3/kubecord-installer) deploy Kubernetes

### 2. Update `/etc/kubernetes/kubelet.env`

1. Add `--allowed-unsafe-sysctls 'kernel.msg*,net.ipv4.*,net.ipv6.*' \` into `/etc/kubernetes/kubelet.env`.
2. `sudo systemctl restart kubelet`
3. `sudo systemctl status kubelet`

### 3. Install NFS Server

On node-1.

```sh
sudo apt-get install -qqy nfs-kernel-server
sudo mkdir -p /nfsshare/mongodb
echo "/nfsshare *(rw,sync,no_root_squash)" | sudo tee /etc/exports
sudo exportfs -r
sudo showmount -e
```

### 4. Install helm

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

### 5. Network Setup

#### 1. Config free5gc.network

```sh
sudo sh -c "cat << EOF > /etc/systemd/network/99-free5gc.netdev
[NetDev]
Name=uptun
Kind=tun
EOF"

sudo systemctl enable systemd-networkd
sudo systemctl restart systemd-networkd

sudo sh -c "echo 'net.ipv6.conf.uptun.disable_ipv6=0' > /etc/sysctl.d/30-free5gc.conf"
sudo sysctl -p /etc/sysctl.d/30-free5gc.conf
sudo sh -c "cat << EOF > /etc/systemd/network/99-free5gc.network
[Match]
Name=uptun
[Network]
Address=45.45.0.1/16
EOF"
```

#### 2. SR-IOV serup

**a. Setup VF num on SR-IOV device & create CRD**

```sh
$ ip a
5: ens11f3: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc mq portid 8cea1b30da42 state UP group default qlen 1000
    link/ether 8c:ea:1b:30:da:42 brd ff:ff:ff:ff:ff:ff
    inet 192.188.2.99/24 brd 192.188.2.255 scope global ens11f3
       valid_lft forever preferred_lft forever
    inet6 fe80::8eea:1bff:fe30:da42/64 scope link
       valid_lft forever preferred_lft forever
$ echo 30 | sudo tee -a /sys/class/net/ens11f3/device/sriov_numvfs
$ kubectl createa -f deploy/crdnetwork.yaml
$ kubectl create -f deploy/sriov-crd.yaml
$ kubectl get net-attach-def
NAME                   AGE
sriov-net1             4h
```

**b. Build and run SRIOV network device plugin**  

Ref: https://github.com/intel/sriov-network-device-plugin#build-and-run-sriov-network-device-plugin  

```sh
$ git clone https://github.com/intel/sriov-network-device-plugin.git && cd sriov-network-device-plugin
$ make
$ make image
```

**c. Setup /etc/pcidp/config.json**  

First, use `lspci | grep -i Ethernet` to check interface's sysfs pci address.  
Then, edit /etc/pcidp/config.json as following:  
```sh
{
    "resourceList":
    [
        {
            "resourceName": "sriov_net",
            "rootDevices": ["01:00.3"],
            "sriovMode": true,
            "deviceType": "netdevice"
        }
    ]
}
```

**d. Deploy sriov-network-device-plugin on k8s**  

```sh
$ kubectl create -f deploy/sriovdp-deploy.yaml
```

**e. Check**  
```sh
$ kubectl get node kubecord-a -o json | jq '.status.allocatable'
{
  "cpu": "31800m",
  "ephemeral-storage": "226240619760",
  "hugepages-1Gi": "8Gi",
  "intel.com/sriov_net": "30",
  "memory": "56931140Ki",
  "pods": "110"
}
```

## Deploy free5GC

### Method 1 - Using yaml

#### 1. Create namespace

```sh
kubectl create -f deploy/nctu5GC/namespace.yaml
```
#### 2. Create mongodb
1. Update `deploy/nctu5GC/mongodb/pv.yaml`
```sh
sed -i "s/192.168.26.11/${NFS_SERVER_IP}/g" deploy/nctu5GC/mongodb/pv.yaml
```
2. Create mongodb
```sh
kubectl create -f deploy/nctu5GC/mongodb/pv.yaml
kubectl create -f deploy/nctu5GC/mongodb/statefulset.yaml
kubectl create -f deploy/nctu5GC/mongodb/service-NP.yaml
```

#### 3. Create webui

```sh
kubectl create -f deploy/nctu5GC/webui-deployment.yaml
kubectl create -f deploy/nctu5GC/webui-service-NP.yaml
```
PS. Using NodePort

Access NODE_IP:31727 via web browser.

**以下請按照 IP 順序 deploy**

#### 4. Deploy free5gc-configmap

```sh
kubectl create -f deploy/nctu5GC/free5gc-configmap.yaml
```

#### 5. Create AMF

> AMF IP: 10.233.84.202

1. Update `deploy/nctu5GC/amf-freediameter-configmap.yaml`
   https://hackmd.io/s/S1baJOeEE#Setup-AMF

2. Deploy
```sh
kubectl create -f deploy/nctu5GC/amf-freediameter-configmap.yaml
kubectl create -f deploy/nctu5GC/amf-deployment-SRIOV.yaml
```

#### 6. Create HSS

> HSS IP: 10.233.84.203

1. Update `deploy/nctu5GC/hss-freediameter-configmap.yaml`
   https://hackmd.io/s/S1baJOeEE#Setup-HSS

2. Deploy
```sh
kubectl create -f deploy/nctu5GC/hss-nextepc-configmap.yaml
kubectl create -f deploy/nctu5GC/hss-freediameter-configmap.yaml
kubectl create -f deploy/nctu5GC/hss-service.yaml
kubectl create -f deploy/nctu5GC/hss-nextepc-deployment.yaml
```


#### 7. Create SMF

> SMF IP: 10.233.84.204

```sh
kubectl create -f deploy/nctu5GC/smf-freediameter-configmap.yaml
kubectl create -f deploy/nctu5GC/smf-service-NP.yaml
kubectl create -f deploy/nctu5GC/smf-deployment.yaml
```

#### 8. Create PCRF

> PCRF IP: 10.233.84.205

1. Update `deploy/nctu5GC/pcrf-freediameter-configmap.yaml`
   https://hackmd.io/s/S1baJOeEE#Setup-PCRF

2. Deploy
```sh
kubectl create -f deploy/nctu5GC/pcrf-freediameter-configmap.yaml
kubectl create -f deploy/nctu5GC/pcrf-nextepc-configmap.yaml
kubectl create -f deploy/nctu5GC/pcrf-service.yaml
kubectl create -f deploy/nctu5GC/pcrf-nextepc-deployment.yaml
```
#### 9. Create UPF

> UPF IP: 10.233.84.206

1. Deploy

```sh
kubectl create -f deploy/nctu5GC/upf-deployment-SRIOV.yaml
```

### Method 2 - Using Helm

To be continued...

---

## Network

```sh

kubectl get network-attachment-definitions
```

## Verification

```sh
$ kubectl get po -n free5gc -o wide
NAME                                       READY   STATUS    RESTARTS   AGE     IP              NODE         NOMINATED NODE
amf-deployment-587f78dc7b-vkbcf            1/1     Running   0          3h53m   10.233.84.202   kubecord-a   <none>
mongo-0                                    1/1     Running   0          2d9h    10.233.69.141   kubecord-a   <none>
nextepc-hss-deployment-77bbb7f986-wj9vf    1/1     Running   0          3h53m   10.233.84.203   kubecord-a   <none>
pcrf-nextepc-deployment-7f6c776d66-6k4b5   1/1     Running   0          3h53m   10.233.84.205   kubecord-a   <none>
smf-deployment-66d85dcd5b-dcvnt            1/1     Running   0          3h53m   10.233.84.204   kubecord-a   <none>
upf-deployment-644d8d8f6-7494d             1/1     Running   5          3h52m   10.233.84.206   kubecord-a   <none>
webui-deployment-6bcfdf7dc9-54khc          1/1     Running   0          2d9h    10.233.69.142   kubecord-a   <none>
```
