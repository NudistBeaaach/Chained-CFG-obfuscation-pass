# Chained-CFG-obfuscation-pass
LLVM obfuscation pass, flattening at the basic block's level and turning each basic block into a dispacher and each instruction into a new basic block.
## Pass description
This CFG flattening pass is applied at the scope of the basic block which means that even programs with few basic block will be obfuscated. Even for a program constituted of a single basic block the result will be fine. This is why this pass can be pretty effective combined with some MBA operation : they can potentially increase the size of each basic block and therefore the number of case in the basic block's dispacher.

* On the left we can see a non obfuscated binary
* On the right an obfuscated binary using the chained CFG flattening pass.

![Non obfuscated binary](./screenshots/NonObf.PNG)
![Non obfuscated binary](./screenshots/Obf.PNG)

The raw size of the file is not much larger (in this case 20 % larger) but the graph is drastically different with much more basic bloc which symbolize a single program's instruction, the other part of the basic block is a xor operation on the local variable used by the dispacher to dinamically find his path in this giant mess.

## Build this pass
This pass uses the new pass manager which means you can build it with cmake (see this [tutorial](https://github.com/banach-space/llvm-tutor#helloworld-your-first-pass) and load it via ```opt``` using this command:
```path/to/llvm/bin/opt -load-pass-plugin /build/folder/libPassTheFishe.so -passes=Pass-Fishe -O0 input.ll -o output.ll```
