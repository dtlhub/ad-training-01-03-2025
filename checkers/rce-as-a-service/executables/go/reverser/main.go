package main

import (
	"encoding/base64"
	"fmt"
	"os"
)

func reverseString(s string) string {
	runes := []rune(s)
	for i, j := 0, len(runes)-1; i < j; i, j = i+1, j-1 {
		runes[i], runes[j] = runes[j], runes[i]
	}
	return string(runes)
}

func main() {
	if len(os.Args) != 2 {
		fmt.Fprintf(os.Stderr, "Usage: %s <base64_string>\n", os.Args[0])
		os.Exit(1)
	}

	decoded, err := base64.StdEncoding.DecodeString(os.Args[1])
	if err != nil {
		fmt.Fprintf(os.Stderr, "Error decoding base64: %v\n", err)
		os.Exit(1)
	}

	reversed := reverseString(string(decoded))
	fmt.Print(reversed)
}