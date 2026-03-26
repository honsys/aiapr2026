package main

import (
	"flag"
	"fmt"
	"log"
	"os"

	"doconvrt/converter"
)

func main() {
	outputFile := flag.String("o", "", "Output file (required).")
	flag.Parse()

	if *outputFile == "" || flag.NArg() != 1 {
		flag.Usage()
		os.Exit(1)
	}
	inputFile := flag.Arg(0)

	if err := converter.ConvertFile(inputFile, *outputFile); err != nil {
		log.Fatalf("Error: %v", err)
	}

	fmt.Printf("Successfully converted '%s' to '%s'\n", inputFile, *outputFile)
}
