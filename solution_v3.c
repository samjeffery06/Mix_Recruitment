/* 
 * Use your preferred naming convention
 * Add comments to help others to understand your code and to document assumptions
 * You are welcome to add additional files if required
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>


struct Node* newNode(uint32_t id, float *pos);
struct Node *insertRec(struct Node *root, uint32_t id, float *pos, uint32_t depth);
struct Node *nearestRec(struct Node *root, float *target, uint32_t depth);
struct Node *nearestNeighbour(struct Node* root, float *target);
struct Node *closest(float *target, struct Node *temp, struct Node *root);

struct Node
{
    uint32_t ID;
    float pos[2];
    struct Node *left, *right;
};

struct Node* newNode(uint32_t id, float *pos)
{
    struct Node *temp = (struct Node*)malloc(sizeof(struct Node));

    temp->ID = id;
    temp->pos[0] = pos[0];
    temp->pos[1] = pos[1];

    temp->left = temp->right = NULL;

    return temp;
}

struct Node *insertRec(struct Node *root, uint32_t id, float *pos, uint32_t depth)
{
    if (root == NULL)
    {
        return newNode(id, pos);
    }

    uint8_t cd = depth % 2;

    if (pos[cd] < (root->pos[cd]))
    {
        root->left = insertRec(root->left, id, pos, depth + 1);
    }
    else
    {
        root->right = insertRec(root->right, id, pos, depth + 1);
    }
    
    return root;
}

struct Node *nearestRec(struct Node *root, float *target, uint32_t depth)
{
    if (root == NULL)
        return NULL;

    // Base cases
    struct Node *nextBranch;
    struct Node *otherBranch;

    uint8_t cd = depth % 2;

    if (target[cd] < root->pos[cd])
    {
        nextBranch = root->left;
        otherBranch = root->right;
    }
    else
    {
        nextBranch = root->right;
        otherBranch = root->left;
    }

    struct Node *temp = nearestRec(nextBranch, target, depth + 1);
    struct Node *best = closest(target, temp, root);

    double radSquared = pow(target[0]- best->pos[0], 2) + pow(target[1] - best->pos[1], 2);

    if (radSquared >= pow(target[cd] - root->pos[cd], 2))
    {
        // printf("Travelling down other path\n");
        temp = nearestRec(otherBranch, target, depth + 1);
        best = closest(target, temp, best);
    }

    return best;
}

struct Node *nearestNeighbour(struct Node* root, float *target)
{
    // Pass current depth as 0
    return nearestRec(root, target, 0);
}

struct Node *closest(float *target, struct Node *temp, struct Node *root)
{
    if (temp == NULL)
    {    
        return root;
    }

    double dist1 = pow(temp->pos[0]-target[0], 2) + pow(temp->pos[1]-target[1], 2);
    double dist2 = pow(root->pos[0]-target[0], 2) + pow(root->pos[1]-target[1], 2);
    
    if (dist1 < dist2)
    {
        return temp;
    }
    return root;
}

int main()
{
    clock_t begin = clock();

    FILE *file;

    struct Node *root = NULL;

    char reg;
    uint32_t ID;
    float pos[2];
    uint64_t timestamp;

    file = fopen("VehiclePositions.dat","rb");
    
    if (file == NULL) {
        printf("Error opening file");
        return 1;
    }

    while (!feof(file))
    {
        fread(&ID, sizeof(uint32_t), 1, file);

        do
        {
            fread(&reg, sizeof(char), 1, file);
        } while (reg != 0x00);

        fread(pos, sizeof(float), 2, file);

        fread(&timestamp, sizeof(uint64_t), 1, file);
        
        // float dist = pow(pos[0]-target[0], 2) + pow(pos[1]-target[1], 2);
        // printf("ID: %d, Lat: %.2f, Lon: %.2f\n", ID, pos[0], pos[1]);
        // printf("Distance for ID %d => %f\n", ID, dist);

        root = insertRec(root, ID, pos, 0);
    }

    /*
    Having built the k-d tree (2d tree)
    Search for the nearest point and return the ID

    Input is a position
    Output is an ID
    */

    float target[10][2] =  {{(float) 34.544909,(float) -102.100843}, {(float)32.345544,(float) -99.123124}, {(float)33.234235, (float)-100.214124}, {(float)35.195739, (float)-95.348899}, {(float)31.895839, (float)-97.789573}, {(float)32.895839, (float)-101.789573}, {(float)34.115839, (float)-100.225732}, {(float)32.335839, (float)-99.992232}, {(float)33.535339, (float) -94.792232}, {(float)32.234235, (float)-100.222222}};

    for(uint8_t i = 0; i < 10; i++)
    {
        struct Node *result = nearestNeighbour(root, target[i]);
        // double dist = pow(result->pos[0]-target[i][0], 2) + pow(result->pos[1]-target[i][1], 2);

        printf("%d: %d\n", i+1, result->ID);
        // printf("Target Lat: %.3f, Target Lon: %.3f\n", target[i][0], target[i][1]);
        // printf("Result found ID: %d, Lat: %.3f, Lon: %.3f\n", result->ID, result->pos[0], result->pos[1]);
        // printf("Result Distance => %f\n", dist);
    }

    // Close the file
    fclose(file);

    clock_t end = clock();

    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

    printf("Clocks per second %d, Clocks %ld, Total Time: %.5f\n", CLOCKS_PER_SEC, end - begin, time_spent);

    return 0;
}