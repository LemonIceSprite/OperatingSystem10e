#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void printvec(int *vec, int n)
{
    for (int i = 0; i < n; i++)
    {
        printf("%d ",vec[i]);
    }
    printf("\n");
}

/* return number of missing times */
int FIFO(int *request, int number_request, int *frame, int number_frame)
{
    int miss = 0;
    int last_replace = 0;
    for (int k = 0; k < number_request; k++)
    {
        int find = 0;
        for (int i = 0; i < number_frame; i++)
        {
            if (frame[i] == request[k])
            {
                find = 1; /* find page in frame */
            }
        }
        if (find == 0)
        {
            int empty = 0;
            for (int i = 0; i < number_frame; i++)
            {
                if (frame[i] == -1)
                {
                    frame[i] = request[k];
                    miss++;
                    empty = 1;
                    last_replace = i;
                    break;
                }
            }
            if (empty == 0)
            {
                last_replace = (last_replace+1)%number_frame;
                frame[last_replace] = request[k];
                miss++;
            }
        }
        // printvec(frame,number_frame);
    }
    return miss;
}

/* return number of missing times */
int LRU(int *request, int number_request, int *frame, int *flag_frame, int number_frame)
{
    memset(flag_frame, 0, sizeof(int) * number_frame);
    int miss = 0;
    for (int k = 0; k < number_request; k++)
    {
        for (int i = 0; i < number_frame; i++)
        {
            flag_frame[i]++;
        }
        int find = 0;
        for (int i = 0; i < number_frame; i++)
        {
            if (frame[i] == request[k])
            {
                find = 1; /* find page in frame */
                flag_frame[i] = 0;
            }
        }
        if (find == 0)
        {
            miss++;
            int empty = 0;
            for (int i = 0; i < number_frame; i++)
            {
                if (frame[i] == -1)
                {
                    frame[i] = request[k];
                    flag_frame[i] = 0;
                    empty = 1;
                    break;
                }
            }
            if (empty == 0)
            {
                int max = flag_frame[0];
                int maxidx = 0;
                for (int i = 1; i < number_frame; i++)
                {
                    if (max < flag_frame[i])
                    {
                        max = flag_frame[i];
                        maxidx = i;
                    }
                }
                frame[maxidx] = request[k];
                flag_frame[maxidx] = 0;
            }
        }
        // printf("frame:");
        // printvec(frame, number_frame);
        // printf("flag :");
        // printvec(flag_frame, number_frame);
    }
    return miss;
}

int OPA(int *request, int number_request, int *frame, int *flag_frame, int number_frame)
{
    memset(flag_frame, 0, sizeof(int) * number_frame);
    int miss = 0;
    for (int k = 0; k < number_request; k++)
    {
        int find = 0;
        for (int i = 0; i < number_frame; i++)
        {
            if (frame[i] == request[k])
            {
                find = 1; /* find page in frame */
            }
        }
        if (find == 0)
        {
            miss++;
            int empty = 0;
            for (int i = 0; i < number_frame; i++)
            {
                if (frame[i] == -1)
                {
                    frame[i] = request[k];
                    empty = 1;
                    break;
                }
            }
            if (empty == 0)
            {
                for (int i = 0; i < number_frame; i++)
                {
                    int count = 0;
                    for (int j = k+1; j < number_request; j++)
                    {
                        if (frame[i] == request[j])
                        {
                            break;
                        }
                        count++;
                    }
                    flag_frame[i] = count;
                }
                int max = flag_frame[0];
                int maxidx = 0;
                for (int i = 1; i < number_frame; i++)
                {
                    if (max < flag_frame[i])
                    {
                        max = flag_frame[i];
                        maxidx = i;
                    }
                }
                frame[maxidx] = request[k];
            }
        }
        // printf("frame:");
        // printvec(frame, number_frame);
        // printf("flag :");
        // printvec(flag_frame, number_frame);
    }
    return miss;
}
int main(int argc, char **argv)
{
    if (argc < 3)
    {
        printf("The input should be : arrange_of_page number_of_request number_of_frame\n");
        return 0;
    }
    int arrange_page = atoi(argv[1]);
    int number_request = atoi(argv[2]);
    int number_frame = atoi(argv[3]);
    int misstime;
    int *request  = (int *)malloc(sizeof(int) * number_request);
    /* -1 represent empty */
    int *frame = (int *)malloc(sizeof(int) * number_frame);
    /* for LRU & OPA */
    int *flag_frame = (int *)malloc(sizeof(int) * number_frame);
    /* produce random requset */
    for (int i = 0; i < number_request; request[i++] = rand()%(arrange_page+1));
    
    // int sample[]={7,0,1,2,0,3,0,4,2,3,0,3,2,1,2,0,1,7,0,1};
    // memcpy(request,sample,sizeof(int) * 20);
    
    printvec(request, number_request);
    /* clean the frame */
    for (int i = 0; i < number_frame; frame[i++] = -1);

    misstime = FIFO(request, number_request, frame, number_frame);
    printf("Missing time of FIFO is : %d\n",misstime);

    /* clean the frame */
    for (int i = 0; i < number_frame; frame[i++] = -1);
    misstime = LRU(request, number_request, frame, flag_frame, number_frame);
    printf("Missing time of LRU is : %d\n",misstime);

    /* clean the frame */
    for (int i = 0; i < number_frame; frame[i++] = -1);
    misstime = OPA(request, number_request, frame, flag_frame, number_frame);
    printf("Missing time of OPA is : %d\n",misstime);

    free(request);
    free(frame);
    free(flag_frame);
    return 1;
}