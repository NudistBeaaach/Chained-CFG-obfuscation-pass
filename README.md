# Chained-CFG-obfuscation-pass
LLVM obfuscation pass, flattening at the basic block's level and turning each basic block into a dispacher and each instruction into a new basic block.
## Pass description
This CFG flattening pass is applied at the scope of the basic block which means that even programs with few basic block will be obfuscated. Even for a program constituted of a single basic block the result will be fine. This is why this pass can be pretty effective combined with some MBA operation : they can potentially increase the size of each basic block and therefore the number of case in the basic block's dispacher.

![Non obfuscated binary](./screenshots/NonObf.PNG)
testaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
![Non obfuscated binary](./screenshots/Obf.PNG)
