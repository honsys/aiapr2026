package main

import (
	"fmt"
	"os"
	"os/exec"
	"path/filepath"
	"strings"
)

func main() {
	// 1. Check if pandoc is installed
	if _, err := exec.LookPath("pandoc"); err != nil {
		fmt.Fprintln(os.Stderr, "Error: pandoc is not installed or not in the system's PATH.")
		os.Exit(1)
	}

	// 2. Validate command-line arguments
	if len(os.Args) != 2 {
		fmt.Fprintf(os.Stderr, "Usage: %s <input.md>\n", os.Args[0])
		os.Exit(1)
	}

	inputFile := os.Args[1]

	// 3. Check if the input file exists
	if _, err := os.Stat(inputFile); os.IsNotExist(err) {
		fmt.Fprintf(os.Stderr, "Error: File not found: %s\n", inputFile)
		os.Exit(1)
	}
	
	if !strings.HasSuffix(inputFile, ".md") {
		fmt.Fprintf(os.Stderr, "Error: Input file must be a Markdown file (.md): %s\n", inputFile)
		os.Exit(1)
	}

	outputFile := strings.TrimSuffix(inputFile, filepath.Ext(inputFile)) + ".tex"

	// 4. Construct and run the pandoc command
	cmd := exec.Command("pandoc",
		inputFile,
		"-s", // Standalone document
		"-o",
		outputFile,
		"--variable", "fontsize=14pt",
		"--variable", "documentclass=extarticle",
		"--variable", "geometry:margin=1in",
	)

	// Capture and print any errors from the command
	output, err := cmd.CombinedOutput()
	if err != nil {
		fmt.Fprintf(os.Stderr, "Error during pandoc conversion:\n%s\n", string(output))
		os.Exit(1)
	}

	fmt.Printf("Successfully converted '%s' to '%s'\n", inputFile, outputFile)
}
