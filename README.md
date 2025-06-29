# Tisp

Tisp is a general-purpose scripting language designed for clarity, control, and conciseness. It follows a block-based syntax with explicit `end` delimiters and aims to blend expressive high-level constructs with predictable semantics and performance.

This language is still under development.

---

## Roadmap Goals

- Block-based, indentation-free syntax
- Lightweight, embeddable runtime
- Strong REPL and script-execution mode
- Minimal standard library with modular extensions
- Structural types, method dispatch, and templates
- Module system with import selectors
- Compile-to-bytecode + direct interpreter mode
- Predictable performance and memory model
- Clear error messages and diagnostics

---

## Examples

```tisp
func main():
    println("Hello, world!");
end
```
```
fun fib(n):
    if n <= 1:
        return n;
    end
    return fib(n - 1) + fib(n - 2);
end

func main():
    println(fib(10));
end
```
```
type Person:
	name: string;
	age : int;
end

let me = Person("Dry", 17);

func main():
    me.sayhi();
end
```
```
func fact(n)
    result = 1;
    i = 2;
    while i <= n:
        result = result * i;
        i = i + 1;
    end
    return result;
end

func main()
    println(fact(5));
end
```
```
func main(args):
    if args.len == 0:
        println("Usage: tisp <filename>");
        exit(1);
    end

    file = readAll(args.at(1));
    if file.is_open():
        println(file.content);
    else
        println("Unable to open file");
    end
end

func add(x, y): x + y end
func greet(name): println("Hello,", name); end
```

---

Philosophy

No significant whitespace

Explicit control flow and scoping

Designed for both scripting and embedding

Syntax-first, but runtime-aware

One-pager simplicity with room to grow



---

Status

Tisp is not ready for use. Everything is subject to change. Development is ongoing.

Contributions, designs, mockups, or experiments are welcome.




