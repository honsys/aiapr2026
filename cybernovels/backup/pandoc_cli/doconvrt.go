package main

import (
	"flag"
	"fmt"
	"os"
	"os/exec"
)

func main() {
	outputFile := flag.String("o", "", "Output file (required).")
	flag.Usage = func() {
		fmt.Fprintf(os.Stderr, "A universal document converter using pandoc.\n")
		fmt.Fprintf(os.Stderr, "Usage: %s [options] <input-file>\n", os.Args[0])
		flag.PrintDefaults()
	}
	flag.Parse()

	if *outputFile == "" || flag.NArg() != 1 {
		flag.Usage()
		os.Exit(1)
	}
	
	inputFile := flag.Arg(0)

	if _, err := exec.LookPath("pandoc"); err != nil {
		fmt.Fprintln(os.Stderr, "Error: pandoc is not installed or not in the system's PATH.")
		os.Exit(1)
	}

	cmd := exec.Command("pandoc", inputFile,
		"-o", *outputFile,
		"--pdf-engine=pdflatex",
		"-V", "fontsize=12pt",
		"-V", "documentclass=article",
		"-V", "geometry:margin=1in",
	)

	stderr, err := cmd.CombinedOutput()
	if err != nil {
		fmt.Fprintf(os.Stderr, "Error converting file:\n%s\n", stderr)
		os.Exit(1)
	}

	fmt.Printf("Successfully converted '%s' to '%s'\n", inputFile, *outputFile)
}
