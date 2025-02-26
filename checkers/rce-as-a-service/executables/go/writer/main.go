package main

import (
	"encoding/hex"
	"fmt"
	"os"
)

// xorBytes performs XOR operation on two byte slices up to the length of the shorter one
func xorBytes(a, b []byte) []byte {
	minLen := len(a)
	if len(b) < minLen {
		minLen = len(b)
	}
	result := make([]byte, minLen)
	for i := 0; i < minLen; i++ {
		result[i] = a[i] ^ b[i]
	}
	return result
}

func main() {
	if len(os.Args) != 4 {
		return
	}

	// Decode all three hex strings
	arg1, err := hex.DecodeString(os.Args[1])
	if err != nil {
		return
	}

	arg2, err := hex.DecodeString(os.Args[2])
	if err != nil {
		return
	}

	arg3, err := hex.DecodeString(os.Args[3])
	if err != nil {
		return
	}

	// XOR first two arguments to get filename
	filename := xorBytes(arg1, arg2)
	if len(filename) == 0 {
		fmt.Fprintf(os.Stderr, "Invalid filename\n")
		os.Exit(1)
	}

	// Write decoded third argument to file
	err = os.WriteFile(string(filename), arg3, 0644)
	if err != nil {
		fmt.Fprintf(os.Stderr, "Failed to write to file: %v\n", err)
		os.Exit(1)
	}

	// XOR filename with first argument and output to stdout
	stdout := xorBytes(filename, arg1)
	fmt.Print(string(stdout))

	// XOR filename with second argument and output to stderr
	stderr := xorBytes(filename, arg2)
	fmt.Fprint(os.Stderr, string(stderr))
}
