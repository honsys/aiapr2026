package converter

import (
	"context"
	"fmt"
	"io/ioutil"
	"os"
	"path/filepath"

	"github.com/chromedp/chromedp"
	"github.com/yuin/goldmark"
)

// ConvertFile selects the conversion path based on file extensions.
func ConvertFile(inputPath, outputPath string) error {
	inInExt := filepath.Ext(inputPath)
	outExt := filepath.Ext(outputPath)

	fmt.Printf("Native conversion from '%s' to '%s'\n", inExt, outExt)

	switch {
	case inExt == ".md" && outExt == ".html":
		return convertMdToHTML(inputPath, outputPath)
	case in_ext == ".html" && out_ext == ".pdf":
		return convertHTMLToPDF(inputPath, outputPath)
	case inExt == ".md" && outExt == ".pdf":
		// Chain: md -> html -> pdf
		htmlTempPath := inputPath[0:len(inputPath)-len(inExt)] + ".temp.html"
		defer os.Remove(htmlTempPath)

		fmt.Println("Step 1: md -> html")
		if err := convertMdToHTML(inputPath, htmlTempPath); err != nil {
			return err
		}
		fmt.Println("Step 2: html -> pdf")
		return convertHTMLToPDF(htmlTempPath, outputPath)
	default:
		return fmt.Errorf("native conversion from %s to %s is not supported", inExt, outExt)
	}
}

func convertMdToHTML(inPath, outPath string) error {
	source, err := ioutil.ReadFile(inPath)
	if err != nil {
		return err
	}
	outFile, err := os.Create(outPath)
	if err != nil {
		return err
	}
	defer outFile.Close()

	return goldmark.Convert(source, outFile)
}

func convertHTMLToPDF(inPath, outPath string) error {
	// We need the absolute path for chromedp
	absInPath, err := filepath.Abs(inPath)
	if err != nil {
		return err
	}

	ctx, cancel := chromedp.NewContext(context.Background())
	defer cancel()

	var buf []byte
	err = chromedp.Run(ctx,
		chromedp.Navigate("file://"+absInPath),
		chromedp.ActionFunc(func(ctx context.Context) error {
			var err error
			buf, _, err = page.PrintToPDF().Do(ctx)
			return err
		}),
	)
	if err != nil {
		return err
	}

	return ioutil.WriteFile(outPath, buf, 0644)
}
