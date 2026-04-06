package main

// Change the target lang on line 26

import (
	"bufio"
	"encoding/json"
	"fmt"
	"io"
	"net/http"
	"net/url"
	"os"
	"strings"
)

func main() {
	reader := bufio.NewReader(os.Stdin)
	input, _ := io.ReadAll(reader)
	text := strings.TrimSpace(string(input))
	if text == "" {
		fmt.Println("(no input)")
		return
	}

	// Change target lang here!
	targetLang := "en"

	escaped := url.QueryEscape(text)
	apiURL := fmt.Sprintf(
		"https://translate.googleapis.com/translate_a/single?client=gtx&sl=auto&tl=%s&dt=t&q=%s",
		targetLang, escaped,
	)

	resp, err := http.Get(apiURL)
	if err != nil {
		fmt.Fprintf(os.Stderr, "error: %v\n", err)
		os.Exit(1)
	}
	defer resp.Body.Close()

	var data interface{}
	if err := json.NewDecoder(resp.Body).Decode(&data); err != nil {
		fmt.Fprintf(os.Stderr, "decode error: %v\n", err)
		os.Exit(1)
	}

	arr, ok := data.([]interface{})
	if !ok || len(arr) == 0 {
		fmt.Fprintf(os.Stderr, "unexpected response\n")
		os.Exit(1)
	}

	first, ok := arr[0].([]interface{})
	if !ok || len(first) == 0 {
		fmt.Fprintf(os.Stderr, "unexpected structure\n")
		os.Exit(1)
	}

	var translated strings.Builder
	for _, p := range first {
		if seg, ok := p.([]interface{}); ok && len(seg) > 0 {
			if str, ok := seg[0].(string); ok {
				translated.WriteString(str)
			}
		}
	}

	fmt.Print(translated.String())
}
