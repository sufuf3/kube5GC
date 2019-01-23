# Modify from: https://raw.githubusercontent.com/acetcom/nextepc/9a86d4cb7cd7b574ec7eb9999c188559c4168bf1/support/docker/webui/Dockerfile

FROM node:carbon

ARG PACKAGE=nextepc

RUN cd /usr/src && git clone https://github.com/sufuf3/kube5GC.git nextepc
RUN cd /usr/src/nextepc && git checkout nctu5gc

WORKDIR /usr/src/nextepc/webui
RUN npm install && \
    npm run build

CMD npm run start

EXPOSE 3000
