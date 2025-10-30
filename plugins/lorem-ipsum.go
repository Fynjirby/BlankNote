package main

import (
	"fmt"
	"io"
	"os"
)

func main() {
	_, _ = io.ReadAll(os.Stdin)
	lorem := `Lorem ipsum dolor sit amet, consectetur adipiscing elit. 
Sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.`
	fmt.Print(lorem)
}
