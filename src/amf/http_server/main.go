package main

import (
	"bytes"
	"crypto/tls"
	"fmt"
	"io/ioutil"
	"log"
	"net"
	"net/http"
	"sync"
	"syscall"

	"golang.org/x/net/http2"
)

var wg sync.WaitGroup

var err error
var client http.Client

var (
	smContextCreateConn   *net.UnixConn
	smContextUpdateConn   *net.UnixConn
	smContextReleaseConn  *net.UnixConn
	smContextRetrieveConn *net.UnixConn
)

var (
	smContextCreateLaddr = net.UnixAddr{
		Name: "/tmp/amf_smContextCreate.gsock",
		Net:  "unixgram",
	}
	smContextCreateRaddr = net.UnixAddr{
		Name: "/tmp/amf_smContextCreate.csock",
		Net:  "unixgram",
	}
	smContextUpdateLaddr = net.UnixAddr{
		Name: "/tmp/amf_smContextUpdate.gsock",
		Net:  "unixgram",
	}
	smContextUpdateRaddr = net.UnixAddr{
		Name: "/tmp/amf_smContextUpdate.csock",
		Net:  "unixgram",
	}
	smContextReleaseLaddr = net.UnixAddr{
		Name: "/tmp/amf_smContextRelease.gsock",
		Net:  "unixgram",
	}
	smContextReleaseRaddr = net.UnixAddr{
		Name: "/tmp/amf_smContextRelease.csock",
		Net:  "unixgram",
	}
	smContextRetrieveLaddr = net.UnixAddr{
		Name: "/tmp/amf_smContextRetrieve.gsock",
		Net:  "unixgram",
	}
	smContextRetrieveRaddr = net.UnixAddr{
		Name: "/tmp/amf_smContextRetrieve.csock",
		Net:  "unixgram",
	}
)

func init() {
	wg.Add(4)
	client = http.Client{
		// Skip TLS dial
		Transport: &http2.Transport{

			TLSClientConfig: &tls.Config{InsecureSkipVerify: true},
		},
	}

	syscall.Unlink(smContextCreateLaddr.String())
	smContextCreateConn, err = net.DialUnix("unixgram", &smContextCreateLaddr, &smContextCreateRaddr)
	if err != nil {
		log.Fatal(err)
		return
	}
	syscall.Unlink(smContextUpdateLaddr.String())
	smContextUpdateConn, err = net.DialUnix("unixgram", &smContextUpdateLaddr, &smContextUpdateRaddr)
	if err != nil {
		log.Fatal(err)
		return
	}
	syscall.Unlink(smContextReleaseLaddr.String())
	smContextReleaseConn, err = net.DialUnix("unixgram", &smContextReleaseLaddr, &smContextReleaseRaddr)
	if err != nil {
		log.Fatal(err)
		return
	}
	syscall.Unlink(smContextRetrieveLaddr.String())
	smContextRetrieveConn, err = net.DialUnix("unixgram", &smContextRetrieveLaddr, &smContextRetrieveRaddr)
	if err != nil {
		log.Fatal(err)
		return
	}
}

func handleSMContextCreate() {
	url := "https://localhost:8080/nsmf-pdusession/v1/sm-contexts"
	var buf []byte
	buf = make([]byte, 1000)
	for {
		_, _, err2 := smContextCreateConn.ReadFromUnix(buf)
		if err2 != nil {
			log.Fatal(err2)
		}
		req, err := http.NewRequest("POST", url, bytes.NewBuffer(buf))
		if err != nil {
			log.Fatal(fmt.Errorf("error making request: %v", err))
		}
		req.Header.Set("Content-Type", "application/json; charset=UTF-8")
		resp, err := client.Do(req)
		if err != nil {
			log.Fatal(fmt.Errorf("error making request: %v", err))
		}
		body, _ := ioutil.ReadAll(resp.Body)
		_, err = smContextCreateConn.Write(body)
		if err != nil {
			log.Fatal(err)
		}
		resp.Body.Close()
	}
	wg.Done()
}

func handleSMContextUpdate() {
	wg.Done()
	// url := "https://localhost:8080/nsmf-pdusession/v1/sm-contexts"
}

func handleSMContextRelease() {
	wg.Done()
	// url := "https://localhost:8080/nsmf-pdusession/v1/sm-contexts"
}

func handleSMContextRetrieve() {
	wg.Done()
	// url := "https://localhost:8080/nsmf-pdusession/v1/sm-contexts"
}

func main() {
	log.Println("Hello")
	go handleSMContextCreate()
	go handleSMContextUpdate()
	go handleSMContextRelease()
	go handleSMContextRetrieve()

	wg.Wait()
}
