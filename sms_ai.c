#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_WORDS 20
#define MAX_LINE 200

char vocabulary[MAX_WORDS][20] = {
"lottery","free","offer","winner","prize",
"urgent","payment","click","reward","claim",
"bank","otp","verify","account","link",
"meeting","hello","project","thanks","tomorrow"
};

int spam_count[MAX_WORDS]={0};
int ham_count[MAX_WORDS]={0};

void to_lower(char str[])
{
    for(int i=0; str[i]; i++)
        str[i]=tolower(str[i]);
}

void train_model()
{
    FILE *file;
    char line[MAX_LINE];
    char label[10], message[150];

    file=fopen("sms_dataset.txt","r");

    if(file==NULL)
    {
        printf("Dataset file not found!\n");
        return;
    }

    printf("Training AI model...\n");

    while(fgets(line,sizeof(line),file))
    {
        sscanf(line,"%[^,],%[^\n]",label,message);

        to_lower(message);

        for(int i=0;i<MAX_WORDS;i++)
        {
            if(strstr(message,vocabulary[i]))
            {
                if(strcmp(label,"spam")==0)
                    spam_count[i]++;
                else
                    ham_count[i]++;
            }
        }
    }

    fclose(file);

    printf("Training completed!\n");
}

int detect_link(char message[])
{
    if(strstr(message,"http") || strstr(message,"www"))
        return 1;

    return 0;
}

int predict(char message[])
{
    float spam_score=1.0;
    float ham_score=1.0;

    for(int i=0;i<MAX_WORDS;i++)
    {
        if(strstr(message,vocabulary[i]))
        {
            spam_score *= (spam_count[i]+1);
            ham_score *= (ham_count[i]+1);
        }
    }

    if(spam_score > ham_score)
        return 1;
    else
        return 0;
}

void update_dataset(char message[], int label)
{
    FILE *file = fopen("sms_dataset.txt","a");

    if(file==NULL)
    {
        printf("Cannot update dataset!\n");
        return;
    }

    if(label==1)
        fprintf(file,"spam,%s",message);
    else
        fprintf(file,"ham,%s",message);

    fclose(file);

    printf("Dataset updated successfully!\n");
}

int main()
{
    char message[200];
    char feedback;
    int correct_label;

    train_model();

    printf("\nEnter SMS message:\n");
    fgets(message,sizeof(message),stdin);

    to_lower(message);

    int result=predict(message);

    if(result || detect_link(message))
        printf("\n⚠ AI Prediction: Fraud / Phishing SMS detected\n");
    else
        printf("\n✅ AI Prediction: Message appears safe\n");

    printf("\nWas this prediction correct? (y/n): ");
    scanf(" %c",&feedback);

    if(feedback=='n' || feedback=='N')
    {
        printf("Enter correct label (1 = spam, 0 = safe): ");
        scanf("%d",&correct_label);

        update_dataset(message,correct_label);
    }

    return 0;
}