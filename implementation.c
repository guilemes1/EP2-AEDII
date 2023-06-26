#include "headers.h"

//Função implementada para criar uma arvore B
bTree* createTree(char* fileName,bool mode)
{
	bTree* tree = malloc(sizeof(bTree));
    
	if(!mode) //new file
    {
        
        strcpy(tree->fileName,fileName);
        tree->fp = fopen(fileName,"w");
        fclose(tree->fp);

        tree->root = 0;
        tree->nextPos = 0;
    } else {
        FILE *oldFile = fopen(fileName, "r");
        fread(tree, sizeof(bTree), 1, oldFile);
        fclose(oldFile);
    }

    tree->fp = fopen(fileName, "r+");
    return tree;
}

bTreeNode* nodeInit(bTreeNode* node,bool isLeaf,bTree* tree)
{
    node->valid = true;
	node->isLeaf = isLeaf;
	node->noOfRecs=0;
	node->pos = tree->nextPos;
	(tree->nextPos)++;
	int i;
	for (int i = 0; i < 2*t; ++i)
	{
		node->children[i] = -1;
	}
	return node;
}

void writeFile(bTree* ptr_tree, bTreeNode* p, int pos) {
    if(pos == -1) {
        pos = ptr_tree->nextPos++;
    }

    fseek(ptr_tree->fp, pos * sizeof(bTreeNode), 0);
    fwrite(p, sizeof(bTreeNode), 1, ptr_tree->fp);
    
}

void readFile(bTree* ptr_tree, bTreeNode* p, int pos) {    
    fseek(ptr_tree->fp, pos * sizeof(bTreeNode), SEEK_SET);
    fread(p, sizeof(bTreeNode), 1, ptr_tree->fp);
}

void enterData(recordNode* record, int codigoLivro, char titulo[], char nomeCompletoPrimeiroAutor[], int anoPublicacao) { //ALTERAR
    
    record->valid = true;
    record->codigoLivro = codigoLivro;
    strcpy(record->titulo, titulo);
    strcpy(record->nomeCompletoPrimeiroAutor, nomeCompletoPrimeiroAutor);
    record->anoPublicacao = anoPublicacao;
    
    return;
}

recordNode* getData(char *filepath, int len) {  //MODIFICADO
    
    recordNode *recordArr = malloc(sizeof(recordNode)*len);
    char delim = ','; char line[256];
    int file_no = 0;
    int i,codigoLivro,anoPublicacao;
    char titulo[255];
    char nomeCompletoPrimeiroAutor[255];
    bool valid;
    
    FILE *inpFile = fopen(filepath, "r");
    
    while(file_no < len && fscanf(inpFile, "%d,%d,%[^,],%[^,],%d", &valid, &codigoLivro, titulo, nomeCompletoPrimeiroAutor, &anoPublicacao)) {
        enterData(&recordArr[file_no], codigoLivro, titulo, nomeCompletoPrimeiroAutor, anoPublicacao);
        file_no++;
    }

    return recordArr;
}

void splitChild(bTree* tree, bTreeNode* x, int i, bTreeNode* y)     //MODIFICADO
{
	bTreeNode* z = malloc(sizeof(bTreeNode));
	nodeInit(z,y->isLeaf,tree);
	z->noOfRecs = t-1;

	int j;
	for(j=0;j<t-1;j++)
	{
		z->keyRecArr[j] = y->keyRecArr[j+t];
        z->posRecArr[j] = y->posRecArr[j+t];      //verificar
	}

	if(!y->isLeaf)
	{
		for(j=0;j<t;j++)
		{
			z->children[j] = y->children[j+t];
            y->children[j+t] = -1; 
		}
	}
	y->noOfRecs = t-1;

	for(j=(x->noOfRecs); j >= i+1;j--)
	{
		x->children[j+1] = x->children[j];
	}
	
	x->children[i+1] = z->pos;

	for(j=(x->noOfRecs) - 1; j >= i;j--)
	{
		x->keyRecArr[j+1] = x->keyRecArr[j];
        x->posRecArr[j+1] = x->posRecArr[j];        //verificar
	}
	x->keyRecArr[i] = y->keyRecArr[t-1];
    x->posRecArr[i] = y->posRecArr[t-1];            //verificar
	x->noOfRecs++;

    writeFile(tree, x, x->pos);
    writeFile(tree, y, y->pos);
    writeFile(tree, z, z->pos);
	free(z);
}

void insertNonFull(bTree* tree,bTreeNode* x,recordNode* record,int* posicao)     //MODIFICADO
{	
	int i = (x->noOfRecs)-1;
	if(x->isLeaf == true)
	{
		while((i>=0) && (record->codigoLivro < x->keyRecArr[i]))
		{
			x->keyRecArr[i+1] = x->keyRecArr[i];
            x->posRecArr[i+1] = x->posRecArr[i];   //verificar
			i--;
		}
		x->keyRecArr[i+1] = record->codigoLivro;
        x->posRecArr[i+1] = (*posicao)++;            //verificar
		(x->noOfRecs)++;

        writeFile(tree, x, x->pos);
	}
	else
	{
		while((i>=0) && (record->codigoLivro < x->keyRecArr[i]))
		{
			i=i-1;
		}
		bTreeNode* childAtPosi = malloc(sizeof(bTreeNode));
        readFile(tree, childAtPosi, x->children[i+1]);
        
		if(childAtPosi->noOfRecs == (2*t-1))
		{
			splitChild(tree,x,i+1,childAtPosi);
			if( x->keyRecArr[i+1] < record->codigoLivro){
				i++;
			}
		}

        readFile(tree, childAtPosi, x->children[i+1]);
		insertNonFull(tree,childAtPosi,record,posicao);

		free(childAtPosi);
	}
}

void insert(bTree* tree,recordNode* record, int* posicao)    //modificado
{
	if(tree->nextPos == 0) 
	{
		tree->root = tree->nextPos;

		bTreeNode* firstNode = malloc(sizeof(bTreeNode));
		nodeInit(firstNode,true,tree);
		firstNode->keyRecArr[0] = record->codigoLivro;
		(firstNode->noOfRecs)++;
        firstNode->posRecArr[0] = (*posicao)++;

        writeFile(tree, firstNode, firstNode->pos);

		free(firstNode);
		return;
	}
	else
	{
		bTreeNode* rootCopy = malloc(sizeof(bTreeNode));
        readFile(tree, rootCopy, tree->root);

		if(rootCopy->noOfRecs == (2*t-1))
		{
			bTreeNode* newRoot = malloc(sizeof(bTreeNode));
			nodeInit(newRoot,false,tree);
			newRoot->children[0] = tree->root;

			splitChild(tree,newRoot,0,rootCopy);

			int i=0;
			if(newRoot->keyRecArr[0] < record->codigoLivro){
				i++;
			}
			
			bTreeNode* childAtPosi = malloc(sizeof(bTreeNode));
            readFile(tree, childAtPosi, newRoot->children[i]);
			insertNonFull(tree,childAtPosi,record,posicao);

			tree->root = newRoot->pos;
            
            

			free(newRoot);
            free(childAtPosi);
		}
		else
		{
			insertNonFull(tree,rootCopy,record,posicao);
		}
		free(rootCopy);
	}
}

void traverse(bTree* tree, int root) {
    
    if(-1 == root) {    
        return;
    }

    bTreeNode *toPrint = malloc(sizeof(bTreeNode));
    readFile(tree, toPrint, root);
    dispNode(toPrint);

    for(int i = 0; i < 2*t; i++) {
        traverse(tree, toPrint->children[i]);
    }

    free(toPrint);
}

void dispNode(bTreeNode* node)      //modificado
{
    printf("Valid: %d\n", node->valid);            //adicionado
	printf("Position in node:%d\n",node->pos );
    printf("Number of Records:%d\n",node->noOfRecs );
	printf("Is leaf?:%d\n",node->isLeaf );
	printf("Keys:\n");
	for(int i = 0; i < node->noOfRecs; i++)
	{
		printf("%d ", node->keyRecArr[i]);
	}
    printf("\nposRec:\n");
    for(int i = 0; i < node->noOfRecs; i++)  //adicionado
	{
		printf("%d ", node->posRecArr[i]);
	}
	printf("\n");
	printf("Links:\n");
	for (int i = 0; i < 2*t; ++i)
	{
		printf("%d ",node->children[i] );
	}
	printf("\n");
    printf("\n");
}

recordNode* searchRecursive(bTree* tree, int key, bTreeNode* root) {  //MODIFICADO
    int i = 0;
    
    while(i < root->noOfRecs && key > root->keyRecArr[i])
        i++;
    
    
    if(i < root->noOfRecs && key == root->keyRecArr[i])
        return root->keyRecArr[i];
    
    
    else if(root->isLeaf) {
        return NULL;
    }
    else {
        bTreeNode* childAtPosi = malloc(sizeof(bTreeNode));
        readFile(tree, childAtPosi, root->children[i]);

        recordNode* found = searchRecursive(tree, key, childAtPosi);  //a funçao devolve um inteiro
        free(childAtPosi);
        return found;
    }
}

recordNode* search(bTree* tree, int key) {
    
    bTreeNode* root = malloc(sizeof(bTreeNode));
    readFile(tree, root, tree->root);

    recordNode* result = searchRecursive(tree, key, root);
    free(root);
    return result;
     
}

int findKey(bTreeNode* node, int k) {
    int idx=0;
    while (idx < node->noOfRecs && node->keyRecArr[idx] < k)
        ++idx;
    return idx;
}

void removeFromLeaf (bTree* tree, bTreeNode *node, int idx) {  //MODIFICADO
    for (int i=idx+1; i<node->noOfRecs; ++i){
	    node->keyRecArr[i-1] = node->keyRecArr[i];
        node->posRecArr[i-1] = node->posRecArr[i];    //verificar
    }
    node->noOfRecs--;
}
 
void removeFromNonLeaf(bTree* tree, bTreeNode *node, int idx) {    //MODIFICAD0
 
    int k = node->keyRecArr[idx];
    
    bTreeNode *child = malloc(sizeof(bTreeNode));
    bTreeNode *sibling = malloc(sizeof(bTreeNode));
    
    readFile(tree, child, node->children[idx]);
    readFile(tree, sibling, node->children[idx+1]); 
 
    
    
    if (child->noOfRecs >= t) {
        recordNode* pred = getPred(tree, node, idx);
        node->keyRecArr[idx] = pred->codigoLivro;       //verificado
        removeNode(tree, child, pred->codigoLivro); 
    }
 
    
    else if  (sibling->noOfRecs >= t)
    {
        recordNode* succ = getSucc(tree, node, idx);
        node->keyRecArr[idx] = succ->codigoLivro;      //vericado
        removeNode(tree, sibling, succ->codigoLivro); 
    }
 
        else {
        child = merge(tree, node, idx); 
        removeNode(tree, child, k);
	    return; 
    }
    
    writeFile(tree, child, child->pos);
    writeFile(tree, sibling, sibling->pos);

    free(child);
    free(sibling);
}


void removeNode(bTree* tree, bTreeNode* node, int k) {

    int idx = findKey(node, k);
    if (idx < node->noOfRecs && node->keyRecArr[idx] == k) {
        if (node->isLeaf){
	        removeFromLeaf(tree, node, idx);
        } else {
            removeFromNonLeaf(tree, node, idx);
        }
        
	    writeFile(tree, node, node->pos);
    }
    else {
       
        if (node->isLeaf) {
		    return false;
       	}
 
        bool flag = idx==node->noOfRecs;
 

        bTreeNode *childAtPosi = malloc(sizeof(bTreeNode));
        readFile(tree, childAtPosi, node->children[idx]);

        if (childAtPosi->noOfRecs < t) {
            fill(tree, node, idx);
            readFile(tree, childAtPosi, node->children[idx]);
        }

        if (flag && idx > node->noOfRecs) {
            bTreeNode *sibling = malloc(sizeof(bTreeNode));
            readFile(tree, sibling, node->children[idx-1]);
            removeNode(tree, sibling, k);

            writeFile(tree, sibling, sibling->pos);
            free(sibling);
        }
        else {
            removeNode(tree, childAtPosi, k);
        }
        
        writeFile(tree, childAtPosi, childAtPosi->pos);
        free(childAtPosi);
    }
}


recordNode* getPred(bTree* tree, bTreeNode *node, int idx) {  //modificado
    bTreeNode *curr = malloc(sizeof(bTreeNode));
    readFile(tree, curr, node->children[idx]);

    while (!curr->isLeaf){
        readFile(tree, curr, curr->children[curr->noOfRecs]);
    }
        
    recordNode* result = curr->children[curr->noOfRecs-1];
    free(curr);
    return result;
}
 
recordNode* getSucc(bTree* tree, bTreeNode *node, int idx) {
 
    bTreeNode *curr = malloc(sizeof(bTreeNode));
    readFile(tree, curr, node->children[idx+1]); 
    while (!curr->isLeaf){
        readFile(tree, curr, curr->children[0]);
    }
 
    
    recordNode* result = curr->posRecArr[0];     //PRESTA ATENCAO
    free(curr);
    return result;
}
 

void fill(bTree* tree, bTreeNode *node, int idx) {
    bTreeNode *cPrev = malloc(sizeof(bTreeNode));
    bTreeNode *cSucc = malloc(sizeof(bTreeNode));

    readFile(tree, cPrev, node->children[idx-1]);
    readFile(tree, cSucc, node->children[idx+1]);
    
    if (idx!=0 && cPrev->noOfRecs>=t) {
        borrowFromPrev(tree, node, idx);
    }
 

    else if (idx!=node->noOfRecs && cSucc->noOfRecs>=t) {
        borrowFromNext(tree, node, idx);
    }
 
    else {
        if (idx != node->noOfRecs)
            merge(tree, node, idx);
        else
            merge(tree, node, idx-1);
    }

    free(cPrev);
    free(cSucc);

    return;
}
 
void borrowFromPrev(bTree* tree, bTreeNode *node, int idx) {
    bTreeNode *child = malloc(sizeof(bTreeNode));
    bTreeNode *sibling = malloc(sizeof(bTreeNode));
    
    readFile(tree, child, node->children[idx]);
    readFile(tree, sibling, node->children[idx-1]);
    

    for (int i=child->noOfRecs-1; i>=0; --i){
        child->keyRecArr[i+1] = child->keyRecArr[i];
        child->posRecArr[i+1] = child->posRecArr[i];    //verificar
    }
 
    if (!child->isLeaf) {
        for(int i=child->noOfRecs; i>=0; --i)
            child->children[i+1] = child->children[i];
    }
 
    child->keyRecArr[0] = node->keyRecArr[idx-1];
    child->posRecArr[0] = node->posRecArr[idx-1];      //verificar
 
    if (!node->isLeaf) {
        child->children[0] = sibling->children[sibling->noOfRecs];
        sibling->children[sibling->noOfRecs] = -1; 
    }
 
    node->keyRecArr[idx-1] = sibling->keyRecArr[sibling->noOfRecs-1];
    node->posRecArr[idx-1] = sibling->posRecArr[sibling->noOfRecs-1];   //verificar
 
    child->noOfRecs += 1;
    sibling->noOfRecs -= 1;
    
    writeFile(tree, node, node->pos);
    writeFile(tree, child, child->pos);
    writeFile(tree, sibling, sibling->pos);
    
    free(child);
    free(sibling);

    return;
}
 
void borrowFromNext(bTree* tree, bTreeNode *node, int idx) {   ///MODIFICADO
 
    bTreeNode *child = malloc(sizeof(bTreeNode));
    bTreeNode *sibling = malloc(sizeof(bTreeNode));
    
    readFile(tree, child, node->children[idx]);
    readFile(tree, sibling, node->children[idx+1]);
    
    child->keyRecArr[(child->noOfRecs)] = node->keyRecArr[idx];
    child->posRecArr[(child->noOfRecs)] = node->posRecArr[idx];     ///verificar
 
    if (!(child->isLeaf))
        child->children[(child->noOfRecs)+1] = sibling->children[0];
 
    node->keyRecArr[idx] = sibling->keyRecArr[0];
    node->posRecArr[idx] = sibling->posRecArr[0];           //verificar
 
    for (int i=1; i<sibling->noOfRecs; ++i){
        sibling->keyRecArr[i-1] = sibling->keyRecArr[i];
        sibling->posRecArr[i-1] = sibling->posRecArr[i];      //verificar
    }
 
    if (!sibling->isLeaf) {
        for(int i=1; i<=sibling->noOfRecs; ++i)
            sibling->children[i-1] = sibling->children[i];

        sibling->children[sibling->noOfRecs] = -1; //RESPOSTA:
    }
 
    child->noOfRecs += 1;
    sibling->noOfRecs -= 1;
    
    writeFile(tree, node, node->pos);
    writeFile(tree, child, child->pos);
    writeFile(tree, sibling, sibling->pos);

    free(child);
    free(sibling);

    return;
}
 
bTreeNode* merge(bTree* tree, bTreeNode *node, int idx) {    //MODIFICADO

    bTreeNode *child = malloc(sizeof(bTreeNode));
    bTreeNode *sibling = malloc(sizeof(bTreeNode));
    
    readFile(tree, child, node->children[idx]);
    readFile(tree, sibling, node->children[idx+1]);
    
    child->keyRecArr[t-1] = node->keyRecArr[idx];
    child->posRecArr[t-1] = node->posRecArr[idx];
 
    for (int i=0; i<sibling->noOfRecs; ++i){
        child->keyRecArr[i+t] = sibling->keyRecArr[i];
        child->posRecArr[i+t] = sibling->posRecArr[i];
    }    
 
    if (!child->isLeaf) {
        for(int i=0; i<=sibling->noOfRecs; ++i)
            child->children[i+t] = sibling->children[i];
    }

    for (int i=idx+1; i<node->noOfRecs; ++i){
        node->keyRecArr[i-1] = node->keyRecArr[i];
        node->posRecArr[i-1] = node->posRecArr[i];
    }
 
    for (int i=idx+2; i<=node->noOfRecs; ++i) 
        node->children[i-1] = node->children[i];
    node->children[node->noOfRecs] = -1; 
    child->noOfRecs += sibling->noOfRecs+1;
    node->noOfRecs--;
 
    if(node->noOfRecs == 0) {
        tree->root = node->children[0];
    }

    writeFile(tree, node, node->pos);
    writeFile(tree, child, child->pos);
    writeFile(tree, sibling, sibling->pos);

    free(sibling);

    return child;
}

bool removeFromTree(bTree* tree, int key) {
    bTreeNode *root = malloc(sizeof(bTreeNode));
    readFile(tree, root, tree->root);

    bool found = search(tree, key);
    if(found);
        removeNode(tree, root, key); 

    free(root);
    return found;
}

void hardPrint(bTree* tree) {
    bTreeNode* lido = (bTreeNode*) malloc(sizeof(bTreeNode));
    for(int i = 0; i < tree->nextPos; i++) {
        fseek(tree->fp, i * sizeof(bTreeNode), SEEK_SET);
        fread(lido, sizeof(bTreeNode), 1, tree->fp);

        if(lido->isLeaf <= 1)
            dispNode(lido);
        else
            printf("ERRO: isLeaf = %i\n\n", lido->isLeaf);
    }

    free(lido);
}

void doublePrint(bTree* tree) {
    printf("=================");
    printf("\nTraverse\n");
    traverse(tree, tree->root);

    printf("=================");
    printf("\nHard print\n");
    hardPrint(tree);
}


//-----------------------------------------------------------------------------


void insertBook(FILE* file, recordNode* record) {
    fseek(file, 0, SEEK_END);  //mover o ponteiro para o final do arquivo
    fwrite(record, sizeof(recordNode), 1, file);
}

void removeBook(FILE * file, int key){
    recordNode* record = malloc(sizeof(recordNode));
	fseek(file, 0, SEEK_SET);       // Mover o ponteiro para o início do arquivo

    while (fread(record, sizeof(recordNode), 1, file) == 1) {
		if (record->valid && record->codigoLivro == key){
			record->valid = false;
			fseek(file, -sizeof(recordNode), SEEK_CUR);
			fwrite(record, sizeof(recordNode), 1, file);
            break;
        }
	}
}

void searchBook(FILE* file, int key){
    
}