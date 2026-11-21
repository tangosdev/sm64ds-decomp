# Decompiling a SM64DS class C++ function in Ghidra 12.0.4

To decompile a Super Mario 64 DS (SM64DS) class C++ function using Ghidra 12.0.4, follow these steps:

1. **Set Up Ghidra**: Ensure you have Ghidra 12.0.4 installed on your system. You can download it from the official Ghidra GitHub repository.
2. **Request access from the maintainers**: Due to security concerns, access for writing the project is restricted. Read-only is more easy to familiarize yourself with the code before making any changes. Just request Andrew (opie-x, one of our core maintainers) a role.
3. **Navigate to the Function**: Once the connection to the server is complete, navigate to the function you want to decompile. You can do this by using the Symbol Tree or by searching for the function name if you have symbol information available. They're in the [config/tu_manifest.d](https://github.com/tangosdev/sm64ds-decomp/tree/main/config/tu_manifest.d) directory of the decompilation project. For example, if you want to decompile the `dCc_c::Process` function, you can find it in the [config/tu_manifest.d/arm9/dCc_c.json](https://github.com/tangosdev/sm64ds-decomp/blob/main/config/tu_manifest.d/arm9/dCc_c.json) file.
4. **Decompile the Function**: Right-click on the function and select "Disassemble - Arm" to view the decompiled code in the listing and decompiler window.
5. **Analyze and Refine**: Analyze the decompiled code and make any necessary adjustments or refinements to improve its readability and accuracy.

Our example function `dCc_c::Process` is located in the directory `src\engine\collision\d_cc.cpp`, [https://github.com/tangosdev/sm64ds-decomp/blob/main/src/engine/collision/d_cc.cpp#L100](https://github.com/tangosdev/sm64ds-decomp/blob/main/src/engine/collision/d_cc.cpp#L100).

![src\engine\collision\d_cc.cpp](images/Decompiled-dCc_c-Process-in-IDE.png)

 You can find the relevant code snippet below:
```cpp
void dCc_c::Process() {
function_body_here...
}
```

Back to Ghidra, the listing and decompiler windows will show the disassembled and decompiled code for the `dCc_c::Process` function. You can analyze the code, identify variables, and **try to understand while matching** the decompiled code with the reconstructed source to understand the logic implemented in the function.

Default Listing view for `func_02014aa8` (`dCc_c::Process`) in Ghidra:

![func_02014aa8 in Listing](images/Ghidra-Listing-View-02014aa8-before-edits.png)

Default Decompiler view for `func_02014aa8` (`dCc_c::Process`) in Ghidra:

![func_02014aa8 in Decompiler](images/Ghidra-Decompiler-View-02014aa8-before-edits.png)

You might think that the decompiled code is not very readable initially (within reason, that's a fine mess!), but you can improve it by renaming variables and functions, adding comments, and using Ghidra's features to enhance the decompilation output. This will help you better understand the function's logic and how it fits into the overall program. 

Let's take a look at setting up the preparation for function matching and renaming in Ghidra. Reviewing the decompiled source:
```cpp
void dCc_c::Process()
```
We see the `::` scope-resolution operator, which indicates that `Process` is a member function of the `dCc_c` class. This is a common pattern in C++ for defining class methods. Of course, we'll need to create the corresponding class in Ghidra to match this function properly.

## Creating the `dCc_c` class in Ghidra and Editing the Function

To create the `dCc_c` class in Ghidra, follow these steps:

- In the Ghidra Code Browser, navigate to the "Symbol Tree" window. You can open it by going to "Window" > "Symbol Tree" in the menu bar.
- Right-click on the "Classes" node in the Symbol Tree and select "Create Class" from the context menu.

![Creating our dCc_c class](images/Ghidra-Creating-New-Class.png)

A option will appear to enter the new class name with the default being `NewClass`. Type `dCc_c` and ENTER to create it. It should look like this in the Symbol Tree:

![dCc_c class in Symbol Tree](images/Ghidra-dCc_c-Class-in-Symbol.png)

Now we're ready to edit the function, press `F` in the listing window to open the "Function Edit" dialog. In the textbox, change the name from `func_02014aa8` to `Process`. In the "Return Type" field, change it from `undefined` to `void`.

![Seeing the function edit dialog, setting the name and return type](images/Ghidra-Function-Edit-Process.png)

We're not done yet; the scope resolution operator will be used to indicate that `Process` is a member function of the `dCc_c` class. `this` is a pointer to the current instance of the class, and it is passed as an implicit parameter to member functions. However, in this function we set the `Calling Convention` to `__stdcall` due to it being the default calling convention for member functions in C++. Ghidra will use this when the `__thiscall` isn't selected.

![Setting the calling convention](images/Ghidra-Function-Calling-Convention-Edit.png)

### Setting the Class reference

In the `Symbol Tree`, open the function panel to list all the functions in the program. You can do this by going to "Window" > "Functions" in the menu bar. For easier organization, Ghidra sets all of them in folders alphabetically by default. 

![Going to Function Panel](images/Ghidra-Function-Panel.png)

You can find our renamed `Process` function in the list, we can select it, and let's drag it all the way to the `dCc_c` class in the `Symbol Tree`. This will set the class reference for the function, indicating that it is a member of the `dCc_c` class.

![Process function in panel](images/Ghidra-Process-Function-in-Function-Panel.png)

The changes are reflected in the `Listing` and `Decompiler` windows, where the function is now correctly identified as a member of the `dCc_c` class, by virtue of the scope resolution operator `::`. The decompiled code will now show `dCc_c::Process()` instead of just `Process()`, making it clear that this function belongs to the `dCc_c` class.

![Process function compared between Listing and Decompiler](images/Ghidra_Comparison_Listing_Process_Func_Scope-Resolution_Operator.png)

## Renaming Decompiled Function Variables

Now we can actually start renaming variables! Take this case for example, the decompiled code shows a variable named `denominator`, which does not make much sense in the context of the function. We can rename it to something more meaningful based on our understanding of the function's source logic.

Look: 

![Ghidra-Process-Edit-Function_Variable](images/Ghidra-Process-Edit-Function_Variable.png)

With `L` we can rename the local variable to `horizontalDistance`:

![Ghidra_Rename_Local_Variable](images/Ghidra_Rename_Local_Variable.png)

`CTRL+L` allows retyping the data type of the variable. In this case, we can change it from `int` to `fx32`, which is more appropriate for a variable representing a distance.

![Ghidra_Retype_Variable](images/Ghidra_Retype_Variable.png)

For another variable case, also rename `VStack_30` to `separation`. Lucky for us, data type is correct here. One plus, the variable can be inferred to really be a separation distance between two objects in the game world:

![Ghidra_Variable_Inference_var_separation](images/Ghidra_Variable_Inference_var_separation.png)

`Ghidra` *auto*-decompiled source:

```cpp
VStack_30.y = piVar7[1] - piVar3[1];
```

Is directly analogous to the human reconstructed source at `VS Code IDE`:

```cpp
separation.y = otherPosition.y - position->y;
```

But we're not done yet! We can turn our attention to the `Listing` window and see our `separation` variable has a `undefined[12]` data type.

![Ghidra_seeing_separation_var_on_Listing_view](images/Ghidra_seeing_separation_var_on_Listing_view.png)

We can change it to `Vector3` which is a universal project struct representing a 3D vector in the game. Right-click on the variable and select "Set Data Type" > "Choose Data Type..." from the context menu. In the "Choose Data Type" dialog, type `Vector3` and click "OK" or select it in the dropdown.

![Ghidra_Change_Listing_Data_Type](images/Ghidra_Change_Listing_Data_Type.png)

## Exposing hidden local functions variables in Listing and Decompiler windows

The `other` variable, in Ghidra being `pdVar12`, which also serves as an example to show an important implicit, not directly stated feature, that being gradual exposing of decompiled local function variables.

![Ghidra_pdVar12_is_var-other_in_source](images/Ghidra_pdVar12_is_var-other_in_source.png)

We see, `other` is a pointer to `dCc_c` class, being declared at the function start. However, scrolling to the end:

**CORRECT**;
```cpp
{
            dCc_c *nextCollider = listHead->next;
            listHead->prev = 0;
            listHead->next = 0;
            listHead = nextCollider;
        }
```

We see another pointer, `nextCollider`, which is also a pointer to `dCc_c` class! In the IDE source, it's declared at the end, however in Ghidra's decompiler, it's exposed in the variable list at the function top, which presents *all* of the global used variables for that func only.

Problem is, it's hidden in the decompiled code! Scrolling to the end of the function, we see instead `other` incorrectly being used as `nextCollider`, this is how it would look as source code in the IDE:

**INCORRECT**;
```cpp
{
            dCc_c *other = listHead->next;
            listHead->prev = 0;
            listHead->next = 0;
            listHead = other;
        }
```
What in Ghidra once incorrectly was:
```cpp
    other = data_0209cee8->next;
    data_0209cee8->prev = (dCc_c *)0x0;
    data_0209cee8->next = (dCc_c *)0x0;
                    // WARNING: Read-only address (ram,0x0209cee8) is written
    data_0209cee8 = other;
    if (other == (dCc_c *)0x0) {
      return;
    }
```

Now turns into:
```cpp
    pdVar10 = data_0209cee8->next;
    data_0209cee8->prev = (dCc_c *)0x0;
    data_0209cee8->next = (dCc_c *)0x0;
                    // WARNING: Read-only address (ram,0x0209cee8) is written
    data_0209cee8 = pdVar10;
    if (pdVar10 == (dCc_c *)0x0) {
      return;
    }
```
All instances of `other` were unaffected, and now we can properly rename `pdVar10`, it's own entity|variable, to `nextCollider`, and the decompiled code will match the IDE source code.

![Ghidra-nextCollider-rename-var-retype](images/Ghidra-nextCollider-rename-var-retype.png)

## Setting up structs from `TrmpFx_ClearAll` for showing edge cases

Let's take a look at the `TrmpFx_ClearAll` function, formerly `func_ov006_02120ca0`, which is located in the `src\minigames\d_s_mg_trampoline.cpp` file. The function is defined as follows in IDE source code:

```cpp
/* .c-derived member: C linkage for the whole block */
void TrmpFx_ClearAll(void) {
    int i;
    for (i = 0; i < sTrmpFxCount; i++) {
        sTrmpFxPool[i].active = 0;
    }
    sTrmpFxList = 0;
}
```

In Ghidra, the listing view disassembly for this function is as follows:

```arm
                         *******************************************************
                         *                      FUNCTION                       *
                         *******************************************************
                         void __stdcall TrmpFx_ClearAll(void)
           void            <VOID>       <RETURN>
           int             lr:4         i                                  XREF[1]:   arm9_ov006::02
           undefined4      Stack[-0x4]  local_4                            XREF[1]:   arm9_ov006::02
                         /* .c-derived member: C linkage for the whole block */
                         TrmpFx_ClearAll                           XREF[2]:   StartRound:021220c0(c), 
                                                                               func_ov006_021242cc:02124364
    arm9_ov006::02120ca0 00 40 2d      stmdb     sp!,{lr}
               e9
    arm9_ov006::02120ca4 04 d0 4d      sub       sp,sp,#0x4
               e2
    arm9_ov006::02120ca8 50 10 9f      ldr       r1,[->sTrmpFxCount]                        ; = 02142F70h
               e5
    arm9_ov006::02120cac 00 e0 a0      mov       lr,#0x0
               e3
    arm9_ov006::02120cb0 00 00 91      ldr       r0,[r1,#0x0]=>DAT_02142f70
               e5
    arm9_ov006::02120cb4 00 00 50      cmp       r0,#0x0
               e3
    arm9_ov006::02120cb8 0a 00 00      ble       LAB_arm9_ov006__02120ce8
               da
    arm9_ov006::02120cbc 40 00 9f      ldr       r0,[->sTrmpFxPool]                         ; = 02142F68h
               e5
    arm9_ov006::02120cc0 0e c0 a0      mov       r12,lr
               e1
    arm9_ov006::02120cc4 0e 30 a0      mov       r3,lr
               e1
                         LAB_arm9_ov006__02120cc8                  XREF[1]:   arm9_ov006::02120ce4(j)  
    arm9_ov006::02120cc8 00 20 90      ldr       r2,[r0,#0x0]=>DAT_02142f68
               e5
    arm9_ov006::02120ccc 01 e0 8e      add       lr,lr,#0x1
               e2
    arm9_ov006::02120cd0 0c 20 82      add       r2,r2,r12
               e0
    arm9_ov006::02120cd4 b0 32 c2      strh      r3,[r2,#0x20]
               e1
    arm9_ov006::02120cd8 00 20 91      ldr       r2,[r1,#0x0]=>DAT_02142f70
               e5
    arm9_ov006::02120cdc 24 c0 8c      add       r12,r12,#0x24
               e2
    arm9_ov006::02120ce0 02 00 5e      cmp       lr,r2
               e1
    arm9_ov006::02120ce4 f7 ff ff      blt       LAB_arm9_ov006__02120cc8
               ba
                         LAB_arm9_ov006__02120ce8                  XREF[1]:   arm9_ov006::02120cb8(j)  
    arm9_ov006::02120ce8 18 00 9f      ldr       r0=>DAT_02142f64,[->sTrmpFxList]           ; = 02142F64h
               e5
    arm9_ov006::02120cec 00 10 a0      mov       r1,#0x0
               e3
    arm9_ov006::02120cf0 00 10 80      str       r1,[r0,#0x0]=>DAT_02142f64
               e5
    arm9_ov006::02120cf4 04 d0 8d      add       sp,sp,#0x4
               e2
    arm9_ov006::02120cf8 00 40 bd      ldmia     sp!,{lr}=>local_4
               e8
    arm9_ov006::02120cfc 1e ff 2f      bx        lr
               e1
                         DAT_arm9_ov006__02120d00                  XREF[1]:   TrmpFx_ClearAll:02120ca8(R)  
    arm9_ov006::02120d00 70 2f 14      undefined   sTrmpFxCount                               ; = ??
               02
                         DAT_arm9_ov006__02120d04                  XREF[1]:   TrmpFx_ClearAll:02120cbc(R)  
    arm9_ov006::02120d04 68 2f 14      undefined   sTrmpFxPool                                ; = ??
               02
                         DAT_arm9_ov006__02120d08                  XREF[1]:   TrmpFx_ClearAll:02120ce8(R)  
    arm9_ov006::02120d08 64 2f 14      undefined   sTrmpFxList                                ; = ??
               02
```
![Ghidra-TrmpFx_ClearAll-Listing-red-data](images/Ghidra-TrmpFx_ClearAll-Listing-red-data.png)

The decompiled code for `TrmpFx_ClearAll` in Ghidra is as follows:

```cpp
void TrmpFx_ClearAll(void) {
  int iVar1;
  int i;
  
                    // /* .c-derived member: C linkage for the whole block */
  i = 0;
  if (0 < _DAT_02142f70) {
    iVar1 = 0;
    do {
      i += 1;
      *(undefined2 *)(_DAT_02142f68 + iVar1 + 0x20) = 0;
      iVar1 += 0x24;
    } while (i < _DAT_02142f70);
  }
  _DAT_02142f64 = 0;
  return;
}
```
![Ghidra-TrmpFx_ClearAll_decompiler-view](images/Ghidra-TrmpFx_ClearAll_decompiler-view.png)

Looking with more attention, at the listing view image, we can see that the `.bss` *initialized memory* references for `sTrmpFxCount`, `sTrmpFxPool`, and `sTrmpFxList` are red. Something's wrong. When double-clicking on the red references, for example, `=>DAT_02142f70`, a warning:

```text
Address not found in program's memory: 02142f70
```
```arm
    arm9_ov006::02120cb0 00 00 91      ldr       r0,[r1,#0x0]=>DAT_02142f70
               e5
```

Let's go and select the region of the red text, right-click, hover over `References`, and select `Add/Edit...` or press `R`.

![Ghidra-Fix-Red-DAT_02142f70_menus](images/Ghidra-Fix-Red-DAT_02142f70_menus.png)

This window appears:

![Ghidra-Fix-Red-ov006-DAT_Reference-Editor](images/Ghidra-Fix-Red-ov006-DAT_Reference-Editor.png)

We can double click the label, and another sub-window appears, where the problem can be spotted:

![Ghidra-Advanced-Reference-Editor-fix-ov006-DAT](images/Ghidra-Advanced-Reference-Editor-fix-ov006-DAT.png)

The `Address` field is set to `02142f70`, which is correct, however, the main way of accessing the `.bss` section is through the `arm9_ov006.bss` segment, not through `ram:02142f70`. We can change it to `arm9_ov006.bss`, and click `OK`. The red text should disappear, and the decompiled code will now show the correct references to the `.bss` section.

![Ghidra-TrmpFx_ClearAll_bss-sTrmpFxCount-fixed](images/Ghidra-TrmpFx_ClearAll_bss-sTrmpFxCount-fixed.png)

In the decompiled view, we can rename the globals `sTrmpFxCount`, `sTrmpFxPool`, and `sTrmpFxList` to their correct names, and the decompiled code will get closer to the IDE source code:

```text
_DAT_02142f70 -> sTrmpFxCount
_DAT_02142f68 -> sTrmpFxPool
_DAT_02142f64 -> sTrmpFxList
```

![Ghidra-Decompiler-View-TrmpFx_ClearAll-Globals-Rename](images/Ghidra-Decompiler-View-TrmpFx_ClearAll-Globals-Rename.png)