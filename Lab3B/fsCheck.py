#NAME: Devyan Biswas
#EMAIL: devyanbiswas@outlook.com
#ID: 804988161

#information to be "global" as I went through the rows and realized what I needed
import sys, string, locale, csv
exitStatus = 0
totalBlocks = 0
totalInodes = 0
freeBlocks = set() #decided to use sets instead of list, becasue i only needed to check for the inode or block in free, so set is better for that purpose
freeInodes = set()
reservedBlocks = set([0,1,2,3,4,5,6,7,64])
inodeLinks = {}
usedBlocks = {} #this will have sets of sets in it; using {} becasue i have a reference and is easier to work with for the dict of dicts i want
linkChecker = {}
dirInodeInfo = {}
parChildInodes = {}
childParInodes = {}
if __name__ == '__main__':

    #Following is for taking in the file and begin processing it!
    try:
        input = open(sys.argv[1] , "r");
    except:
        sys.stderr.write("Error: file does not exist\n")
        exitStatus = 1
        exit(exitStatus)
    for line in input:
        row = line.split(',') #creates array using the line, entries split by the commas
        #print(row)
        #print(len(row))
        #Getting the relevant information from the different rows
        
        #getting info from da dang superblock
        if row[0] == "SUPERBLOCK":
            totalBlocks = int(row[1])
            totalInodes = int(row[2])


        #getting info from the other row bois
        elif row[0] == "IFREE":
            freeInodes.add(int(row[1]))


        elif row[0] == "BFREE":
            freeBlocks.add(int(row[1]))


        elif row[0] == "INODE":
            currInode = int(row[1])
            inodeLinks[currInode] = int(row[6])
            #need to looka t entries 12 to 27 cause those are the pointers to the blocks: 11 + 15 == max of 26
            for itr in range(12 , 27):
                #the indirect ptrs are located in the 3 before last, as seen in the previous lab in process inode: 12, 13, 14 : 24, 25,26 for this one
                currBlock = int(row[itr])
                levelIndir = ""
                offset = 0
                lvlNum = 0
                if currBlock == 0: #block not used, so no ptr
                    continue;
                if itr == 24: #means the first indirect one is used
                    levelIndir = " INDIRECT"
                    offset = 12
                    lvlNum = 1
                elif itr == 25: #means doubly indirect
                    levelIndir = " DOUBLE INDIRECT"
                    offset = 256 + 12
                    lvlNum = 2
                elif itr == 26: #means triply indirect
                    levelIndir = " TRIPLE INDIRECT"
                    offset = 65536 + 256 + 12
                    lvlNum = 3
                #checks from the spec
                if currBlock < 0 or currBlock > totalBlocks: #check to make sure that the current value for block is logically within the range of block values
                    sys.stdout.write("INVALID" + levelIndir + " BLOCK " + str(currBlock) + " IN INODE " + str(currInode) + " AT OFFSET " + str(offset) + '\n') 
                    exitStatus = 2
                elif currBlock in reservedBlocks and currBlock != 0:#now need to check if the value is valid, ie not in the "reserved" values for filesystem metadata
                    sys.stdout.write("RESERVED" + levelIndir + " BLOCK " + str(currBlock) + " IN INODE " + str(currInode) + " AT OFFSET " + str(offset) + '\n')
                    exitStatus = 2
                chkL = [currInode, lvlNum, offset]
                if currBlock in usedBlocks:
                    usedBlocks[currBlock].append(chkL)
                else:
                    usedBlocks[currBlock] = [chkL]
        
        
        elif row[0] == "INDIRECT":
            currInode = int(row[1]) #vals coming from part of lab, in process inode section
            currBlock = int(row[5]) #vals coming from part of lab, in process inode section
            lvlNum = int(row[2])
            if lvlNum == 1:
                levelIndir = "INDIRECT"
                offset = 12
            elif lvlNum == 2:
                levelIndir = "DOUBLE INDIRECT"
                offset = 256 + 12
            elif lvlNum == 3:
                levelIndir = "TRIPLE INDIRECT"
                offset = 65536 + 256 + 12
            #checks from spec
            if currBlock < 0 or currBlock > totalBlocks: #check to make sure that the current value for block is logically within the range of block values
                sys.stdout.write("INVALID" + levelIndir + " BLOCK " + str(currBlock) + " IN INODE " + str(currInode) + " AT OFFSET " + str(offset) + '\n') 
                exitStatus = 2
            elif currBlock in reservedBlocks and currBlock != 0:#now need to check if the value is valid, ie not in the "reserved" values for filesystem metadata
                sys.stdout.write("RESERVED" + levelIndir + " BLOCK " + str(currBlock) + " IN INODE " + str(currInode) + " AT OFFSET " + str(offset) + '\n')
                exitStatus = 2
            chkL = [currInode, lvlNum, offset]
            if currBlock in usedBlocks:
                usedBlocks[currBlock].append(chkL)
            else:
                usedBlocks[currBlock] = [chkL]
        
        
        elif row[0] == "DIRENT":
            prevInode = int(row[1]) #vals coming from previous part of the lab
            currInode = int(row[3]) #vals coming from previous part of the lab
            name = row[6]
            
            dirInodeInfo[currInode] = name #realized that when going through later on i dont asctually store the corresponding inode and dir name, so here it is

            if currInode not in linkChecker: #inodeLinks[currInode] = int(row[6]) : from inode, this will be corresponding to the value gotten there. This is the manual way of checking number links, in a sense
                linkChecker[currInode] = 1
            else:
                linkChecker[currInode] += 1

            if currInode < 1 or currInode > totalInodes: #another basic check
                sys.stdout.write("DIRECTORY INODE " + str(prevInode) + " NAME " + name[0:len(name)-2] + "' INVALID INODE " + str(currInode) + '\n')
                exitStatus = 2
                continue

            elif name[0:3] == "'.'" : #this was a little trickier to remember, but note that '.' means the same directory, so if they dont match for the directory with name '.', it is a discrepancy
                if prevInode != currInode:
                    sys.stdout.write("DIRECTORY INODE " + str(prevInode) + " NAME " + name[0:3] + " LINK TO INODE " + str(currInode) + " SHOULD BE " + str(prevInode) + '\n')
                    exitStatus = 2
                else:
                    continue
            
            elif name[0:4] == "'..'": #this is another check, since '..' is previous, we will check to make dure these correspond later
                parChildInodes[prevInode] = currInode
            else:
                childParInodes[currInode] = prevInode

#OK! FINALLY done with the annoying setup and normal and reverse checking nonsense for the last one! Time to start doing checks and comparisons!

#A simple check to make sure that the inodeLinks that are used are not on freelist and vice versa
for i in range(1, totalInodes + 1):
    if i in inodeLinks and i in freeInodes:
        sys.stdout.write("ALLOCATED INODE " + str(i) + " ON FREELIST")
        exitStatus = 2
    elif i not in inodeLinks and i not in freeInodes and i not in childParInodes and i not in (1, 3, 4, 5, 6, 7, 8, 9, 10):
        sys.stdout.write("UNALLOCATED INODE " + str(i) + " NOT ON FREELIST")
        exitStatus = 2

#A simple check to make sure that the number of links we got when building up inodeLinks is the same as the one we got building up the linkChecker
#Remember: inodeLinks was gotten from the inodes themsevles, in element 6 of the row array. linkchecker is from the dirent, in which the inode element can be used to check for its number of links/references
for i in inodeLinks:
    links1 = inodeLinks[i]
    if i in linkChecker:
        links2 = linkChecker[i]
    else:
        links2 = 0
    if links1 != links2:
        sys.stdout.write("INODE " + str(i) + " HAS " + str(links2) + " LINKS BUT LINKCOUNT IS " + str(links1) + '\n')
        exitStatus = 2

#A simple check to make dure that no allocated blocks are shown as being free, and any blocks are not being referenced at all
for i in range(1, totalBlocks + 1):
    if i in usedBlocks and i in freeBlocks:
        sys.stdout.write("ALLOCATED BLOCK " + str(i) + " ON FREELIST")
        exitStatus = 2
    elif i not in usedBlocks and i not in freeBlocks and i not in reservedBlocks:
        sys.stdout.write("UNREFERENCED BLOCK " + str(i) + '\n')
        exitStatus = 2

#checking for duplicates in the usedBlocks that we got from INODE and indirect
#special note: can't take credit for this idea, found it from online. But the idea is this: you have this dict of dicts, and if a particular inode has more than one dict in the corresponding place, you know it was referenced mroe than once! It's a special hashmap, thats all it is...
for i in usedBlocks:
    if len(usedBlocks[i]) > 1:
        exitStatus = 2
        for j in usedBlocks[i]: #[currInode, lvlNum, offset] for reference
            lvlNum = int(j[1])
            levelIndir = ""
            if lvlNum == 1:
                levelIndir = " INDIRECT"
            elif lvlNum == 2:
                levelIndir = " DOUBLE INDIRECT"
            elif lvlNum == 3:
                levelIndir = " TRIPLE INDIRECT"
            sys.stdout.write("DUPLICATE" + levelIndir + " BLOCK " + str(i) + " IN INODE " + str(j[0]) + " AT OFFSET " + str(j[2]) + '\n')
            
#checking that there are no unallocated directory inodes that are being referenced from the child -> parent inodes
for i in dirInodeInfo:
    if i in freeInodes and i in childParInodes:
        parent = childParInodes[i]
        nameDir = dirInodeInfo[i]
        sys.stdout.write("DIRECTORY INODE " + str(parent) + " NAME " + name[0:len(name)-2] + "' UNALLOCATED INODE " + str(i) + '\n')
        exitStatus = 2

#Checking that the links from the directory inodes are matching
for i in parChildInodes:
    if i == 2:
        if parChildInodes[i] == 2:
            continue
        else:
            sys.stdout.write("DIRECTORY INODE " + str(i) + " NAME '..' LINK TO INODE " + str(parChildInodes[i]) + " SHOULD BE " + str(i) + '\n')
            exitStatus = 2
    elif i not in childParInodes:
        sys.stdout.write("DIRECTORY INODE " + str(i) +" NAME '..' LINK TI INODE " + str(i) + " SHOULD BE " + str(childParInodes[i]) + '\n')
        exitStatus = 2;
    elif parChildInodes[i] != childParInodes[i]:
        sys.stdout.write("DIRECTORY INODE " + str(i)+ " NAME '..' LINK TO INODE " + str(i) + " SHOULD BE " + str(childParInodes[i]) + '\n')
        exitStatus = 2;


exit(exitStatus)
