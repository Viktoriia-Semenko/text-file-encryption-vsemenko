# Test plan for Assignment 4
## Structure
1. CaesarCiper
   - This class is loading/unloading a library inside and call encrypt/decrypt functions
3. UndoRedoBuffer
   - This class implements the undo/redo algorithm
4. ConsoleInput
   - This class is responsible for all input that user enters. For example name of the file, key for cipher, text ect.
5. FileHandler
   - This class is works with saving/loading files
6. Text
   - responsible for the text storage itself. It has functions that works with text(like insert, append, delete ect.)
7. CommandLineInterface
   - responsible for an interaction with a user
   - allow user to enter the input path to an object they want to encrypt or decrypt. User is able to encrypt or decrypt the input file with specified key.
     - a. user choses type of operation: encrypt or decrypt
     - b. user enters input file path
     - c. user enters output file path user enters the key (in order to encrypt/decrypt the message)
     - d. have a functionality to encrypt/decrypt a separate file
## Case 1
1. Append text (command 1)
> Rozes are red, tulips are white

2. Save text to file (command 3)
> encrypt.txt

3. Encrypt text (command 15)
> Enter 1 to encrypt or 2 to decrypt: 1 </br>
> Enter input file path: encrypt.txt </br>
> Enter output file path: decrypt.tx </br>
> Enter the key: 1 </br>
> decrypt.txt: Spaft bsf sfe, uvmjqt bsf xijuf
