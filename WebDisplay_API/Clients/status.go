package main

import (
	"fmt"
	"net/http"
	"net/url"
    "io/ioutil"
    "github.com/shirou/gopsutil/v3/mem"
)

func main() {
	params := url.Values{}
    Url, err := url.Parse("http://192.168.10.163/api")
    if err != nil {
        return
    }
    params.Set("token","TurboCloudIOT")
	params.Set("clear","1")
	params.Set("cx","0")
	params.Set("cy","4")
	params.Set("size","3")
	params.Set("text","Test!")
    Url.RawQuery = params.Encode()
    urlPath := Url.String()
    fmt.Println(urlPath)
    resp,err := http.Get(urlPath)
    defer resp.Body.Close()
    body, _ := ioutil.ReadAll(resp.Body)
    fmt.Println(string(body))
    fmt.Println(GetCpuPercent())
    
}

func GetCpuPercent() float64 {
	percent, _:= cpu.Percent(time.Second, false)
	return percent[0]
}

func GetMemPercent()float64 {
	memInfo, _ := mem.VirtualMemory()
	return memInfo.UsedPercent
}

func GetDiskPercent() float64 {
	parts, _ := disk.Partitions(true)
	diskInfo, _ := disk.Usage(parts[0].Mountpoint)
	return diskInfo.UsedPercent
}