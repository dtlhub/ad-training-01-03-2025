package main

import (
	"fmt"
	"io"
	"os"
)

func main() {
	if len(os.Args) != 2 {
		fmt.Fprintf(os.Stderr, "Usage: %s <filename>\n", os.Args[0])
		os.Exit(1)
	}

	file, err := os.OpenFile(os.Args[1], os.O_RDONLY, 0)
	if err != nil {
		fmt.Fprintf(os.Stderr, "Error: Could not open file '%s': %v\n", os.Args[1], err)
		os.Exit(1)
	}
	defer file.Close()

	_, err = io.Copy(os.Stdout, file)
	if err != nil {
		fmt.Fprintf(os.Stderr, "Error: Failed to read from file '%s': %v\n", os.Args[1], err)
		os.Exit(1)
	}
}
