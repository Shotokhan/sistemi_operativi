struct childrenType{
        char* name;
        int* pidArray;
        int numChildren;
};

int whichChild(struct childrenType *childType, int numTypes, int pid);

void waitProcesses(struct childrenType *childType, int numTypes, int numProcesses);
