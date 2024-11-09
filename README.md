# Shepherd
> An experimental lexical analyzer unit for C programming language

## Abstract

Shepherd is a combination of `cpp` (aka C preprocessor) and C's lexer, which ultimately serves as C's lexer.

It's a attempt by directly integrating preprocessor into C's lexer, thus C's parser can fetch tokens one-by-one 
without having a standalone `cpp` to generate a preprocessed source file.

Ideally, this can reduce memory usage since we adopted a technique called **Deforestation** (or aka **Fusion**) to
eliminate intermediate data structure, file in this case, and directly pass tokens to parser.

## Building

To build it, please refer to shecc's [prerequisites](https://github.com/sysprog21/shecc/blob/master/README.md#Prerequisites) first,
then you can build it by simply typing `make`. There's more commands than just building:

- `make run`: Rebuilds and runs the Shepherd
- `make clean`: cleans `out` folder
- `make update`: Updates toolchain `shecc` to its latest commit and rebuilds it

## License

`Shepherd` is freely redistributable under the MIT license.
Use of this source code is governed by a MIT license that can be found in the [`LICENSE`](./LICENSE) file.
