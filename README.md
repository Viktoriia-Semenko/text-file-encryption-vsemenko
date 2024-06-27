# Text and File Encryption (Object Oriented Programming)
## Task
In this lab, you are going to upgrade a dynamic-link library (DLL) to allow encrypting and
decrypting byte sequences of any size. Then you should add the logic to read files, encrypt, save
the encrypted one, load and decrypt. This should be encapsulated inside CaesarCipher class (e.g.
it can contain constructor, destructor, encrypt and decrypt methods; inside these methods you can
use platform-specific API like LoadLibrary or dlopen).
<br></br>
In this task you need to define the command line interface by yourself, the best option would be to
extend the existing CLI from the assignment 2.
## My structure of the program
1. CaesarCiper class
   - This class is loading/unloading a library inside and call encrypt/decrypt functions
2. UndoRedoBuffer
   - This class implements the undo/redo algorithm
3. ConsoleInput
   - This class is responsible for all input that user enters. For example name of the file, key for cipher, text ect.
4. FileHandler
   - This class is works with saving/loading files
5. Text
   - responsible for the text storage itself. It has functions that works with text(like insert, append, delete ect.)
6. CommandLineInterface
   - responsible for an interaction with a user
   - allow user to enter the input path to an object they want to encrypt or decrypt. User is able
to encrypt or decrypt the input file with specified key.
      - a. user choses type of operation: encrypt or decrypt
      - b. user enters input file path
      - c. user enters output file path user enters the key (in order to encrypt/decrypt the message)
