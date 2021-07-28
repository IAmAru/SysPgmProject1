Alejandro Jimenez
sai993

So funny story, in the "oh god why" kind of funny.

It might be due to me skimming the document, but I didn't get the memo that we were supposed to use
execvp() to execute commands. This led to a lot of development time that otherwise could've gone to the
needed parts of the project going to me developing the commands from scratch; barring ls-l. This, combined
with the lack of foresight that it really would be a difficult project, sapped away at my time.

It's unfortunate, but I'd rather turn what I have in rather than try to build something from scratch using
things I probably won't get around to understanding for a little while.

Before going into some specifics of what I wrote, I'd like to point out some of what I wasn't able to get done:

======= Faults =======

1. Firstly, and most importantly, I wasn't able to get Batch Mode working. I'd like to believe I was pretty close
to getting it, and I'll describe my thought process:

First, to even tell from interactive and batch mode, it starts an int variable, batchmode at line 24. This one in particular starts at 0.
Now, with that defined, it can tell from two while loops: the one at line 28, and the one at line 258.

When it first boots up, it will do so in interactive mode. From there, to enter batch mode, the user would need to do the command 'batch'.
It makes the aforementioned variable, batchmode equal to one. When it finishes the command, it'll go back to checking both while loop conditions.
Since batchmode is 1, it'll go into the Batch Mode while loop. From there, what was supposed to happen is that the program would ask the user for
a file to read. This is, unfortunately, as far as it goes.

My thought process was the following: 

Open up the user-input file with fopen(), in "r" mode. If it does not exist, throw an error at the user. If it does exist,
open up the file and read the characters. Every character gets added into an array, and if the character just so happens to be \n, it'll stop,
define the array as the standard input for the interactive mode, and go to the interactive mode version of the command.
So, for example, if the file's only line is 'ls', it would jump to the ls definition, and return to batch mode once it was done.

Unfortunately I got a bunch of segmentation faults while trying this out, and errors upon errors. I'd love to try to revisit this at some point, but 
for now I think it'd be better for my sanity if I took the hit.

2. I believe this fault in particular is because I used a separate token from the semicolon one, but some of the commands block any subsequent commands.
These troublemakers would be cat, grep and touch. So, for example, if I were to say ls; ls -l, it'd work out just dandy. However, ls; cat [file]; ls -l would
only do the commands for ls and cat [file], missing the third one entirely. I'm not really sure what to say... I tried to fix it by redefining the semicolon token,
but it didn't seem to want to cooperate. Note, however, that ls; ls -l; cat [file] would work... it's only the commands after cat/grep/wc/touch that don't read.

3. General glitches and some lack of error handling: I tried to do as much as I could for error handling, but some small glitches might've skipped my grasp.
I don't believe them to be too many. As a matter of fact, there might not even be any at all. But I thought it'd be better to point it out in case there were.
This would be due to me writing the commands from scratch, rather than using execvp(). 

4. Obviously... the fact that I wrote the commands myself rather than using execvp().

I would believe that's about it for what I did wrong, or what I missed. From testing with the commands listed in the testing file, I was actually pretty proud
that all but one in interactive mode worked; this being 'cat [file]; grep [string] [file]'.

==== Design choices ====

I'm not going to specify much of my thoughts while designing the commands from scratch because, really, it wasn't supposed to be done that way.
I'll instead focus on how the program uses fgets to gather user input.

Firstly, I defined the input as i[MAX] and put it in an fgets statement. From there, it gathered all the information the user would input. For the sake of it,
let's say this string was 'ls; ls -l; asd asd; ; cat [file]'.

The first thing the program does is remove check for the length in line 53. If it's longer than 512, it tells the user it exceeded the limit, and restarts.
After that, it removes \n from the end of the string for easier access to commands. In truth, I did this because I didn't want to write \n at the end of every
command.

What happens next is that it looks in the string for semicolons, and uses the words behind it as the commands. So, from our token, those commands would be
ls
 ls -l
 asd asd

 cat [file]

The spaces are important there. And they were a pain to deal with. I didn't really know how to do anything there with the spaces. Should I include [space]ls -l
as a readable command? What if it's poorly formatted, and there's two spaces?

First, I wanted to store the original result. So, I called a char* sc_token2. We'll need it later, so it's important for now.
I created a secondary array, called result, and called a for loop. For every character in the token that was not a space, add it to the result array, and then
make the token equal to the result. This made things quite simple- it got rid of every space there was, no matter what. So, our tokens were now

ls
ls-l
asdasd

cat[file]

From here, a new while loop is called, for whenever our tokens run out.
This might look like a weird design choice, but I think it made it for things to be more uniform as far as input goes.
Since ls and ls-l will be constant with their calls, or don't necessarily take any other arguments, this is as far as those go for complications.
In line 71, ls is called and does its thing. Goes to the end of the while loop, where the token is advanced by one, which means now we've iterated 
through ls, and are going to iterate through ls-l.
Same thing happens, and ls -l is defined in line 81. Token is advanced.
Now, since asdasd is not a command, it gets tossed out in line 249. Token advances once more to the blank.
At line 65, the token is checked if it's just blank space. Since we removed all spaces, it is, indeed, just "".
This sets a flag, skipflag, to be 1.
Going to the end of the code, it would normally say "" is not a recognized command, but thanks to the skipflag, we just skip saying that and roll over to the
next token, which is cat[file].

This is where it gets interesting. For ls and ls -l, I used strcmp, but for cat and the other commands I used strncmp(), which allows me to check if a
string starts with a certain bunch of characters, which allows me to get my commands. They'll always be the same length, always be the same string if
they are to be called.

Next, I will use the sc_token2 I mentioned before. This guy would still have all the spaces. So... what now? 
I called a new strtok() to check for space tokens. So, now, using sc_token2 to see what we're going to be dealing with, I'll roll over the token once.
This guarantees that I'll be in the arguments, and skip the command as the token being iterated completely. We would be dealing with [file] now, which is
exactly what we want. We use that [file] argument in the necessary places for the command.

The same concept is applied to the commands I did not call in the example. Please toy around with the shell to see.

I believe that's what I would comment about my design process for the shell. Please give me a good grade.























