GO = go

.PHONY: all clean

all: export GOPATH=$(GOPATH):$(PWD)/.. smf_http_server

smf_http_server: main.go handler.go routers.go
	$(GO) build -o $@ $^

clean:
	$(RM) smf_http_server
