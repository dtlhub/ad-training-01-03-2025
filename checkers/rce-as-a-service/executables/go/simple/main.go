package main

import (
	"fmt"
	"os"
	"strings"
)

func main() {
	if len(os.Args) > 1 {
		fmt.Print(strings.Join(os.Args[1:], " "))
	}
}