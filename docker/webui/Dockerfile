# Modify from: https://raw.githubusercontent.com/acetcom/nextepc/9a86d4cb7cd7b574ec7eb9999c188559c4168bf1/support/docker/webui/Dockerfile

FROM node:carbon

ARG PACKAGE=free5gc

RUN cd /usr/src && git clone https://github.com/sufuf3/kube5GC.git /usr/src/free5gc
RUN cd /usr/src/free5gc && git checkout nctu5gc

WORKDIR /usr/src/free5gc/webui
RUN npm install && \
    npm run build

CMD npm run start

EXPOSE 3000
