#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> 

#define profile_file "the_profile.csv"
#define report_file  "the_health_report.txt"

typedef struct {
    float bmi;
    int bmi_status;
    int bp_status;
    int bs_status;
    int chol_status;
} HealthData;

typedef struct {
    char name[50];
    float weight;
    float height;
    int bp_sys;
    int bp_dias;
    int bs;
    int chol;
    int bs_flag;
    HealthData analysis;
} Profile;

// Function prototypes
HealthData analyzeData(float weight, float height, int bp_sys, int bp_dias, int bs, int chol); 
void dietAddAvoid(HealthData data, FILE *fp);
void exerciseAddAvoid(HealthData data, FILE *fp);

const char *const bmi_labels[] = {
    "Underweight",
    "Normal",
    "Overweight",
    "Obesity Class 1",
    "Obesity Class 2",
    "Obesity Class 3"
};

const char *const bp_labels[] = {
    "Hypotension (Low)",
    "Normal",
    "Elevated",
    "Stage 1 Hypertension",
    "Stage 2 Hypertension",
    "Hypertensive Crisis"
};

const char *const bs_labels[] = {
    "Dangerously Low",
    "Low",
    "Normal",
    "High",
    "Dangerously High"
};

const char *const chol_labels[] = {
    "Low Heart Disease Risk",
    "Borderline Risk",
    "High Risk"
};

// -----------------------------------------
// SAVE PROFILE TO CSV
// -----------------------------------------

void saveProfile(Profile p) {
    FILE* file = fopen(profile_file, "w");
    if (!file) return;

    // Modified fprintf to include the full labels
    fprintf(file, "Name: %s,\nWeight: %.2f,\nHeight: %.2f,\nBlood Pressure Systolic: %d,\nBlood Pressure Diastolic: %d,\nBlood Sugar: %d,\nCholesterol: %d",
            p.name, p.weight, p.height, p.bp_sys, p.bp_dias, p.bs, p.chol);

    fclose(file);
}

// -----------------------------------------
// LOAD PROFILE FROM CSV
// -----------------------------------------

int loadProfile(Profile* p) {
    FILE* file = fopen(profile_file, "r");
    if (!file) return 0;

    fscanf(file, "%49[^,],%f,%f,%d,%d,%d,%d",
           p->name, &p->weight, &p->height,
           &p->bp_sys, &p->bp_dias, &p->bs, &p->chol);

    fclose(file);

    p->analysis = analyzeData(
        p->weight, p->height,
        p->bp_sys, p->bp_dias,
        p->bs, p->chol
    );

    return 1;
}

// -----------------------------------------
// REPORT GENERATOR
// -----------------------------------------

void generateReport(Profile p) {
    FILE* fp = fopen(report_file, "w");

    fprintf(fp, "HEALTH REPORT FOR: %s\n", p.name);
    fprintf(fp, "==============================\n");

    fprintf(fp, "BMI: %.2f (Status: %s)\n",
            p.analysis.bmi,
            bmi_labels[p.analysis.bmi_status]);

    fprintf(fp, "Blood Pressure: %d/%d (%s)\n",
            p.bp_sys, p.bp_dias,
            bp_labels[p.analysis.bp_status]);

    fprintf(fp, "Blood Sugar: %d (%s)\n",
            p.bs,
            bs_labels[p.analysis.bs_status]);

    fprintf(fp, "Cholesterol: %d (%s)\n",
            p.chol,
            chol_labels[p.analysis.chol_status]);

    fclose(fp);

    printf("\n[SUCCESS] Personal report generated in %s\n", report_file);
}

// -----------------------------------------
// ANALYSIS FUNCTION
// -----------------------------------------

HealthData analyzeData(float weight, float height, int bp_sys, int bp_dias, int bs, int chol) {
    HealthData data;

    // Auto detect height in cm or meters
    float h = height;
    if (h > 3.0f) h /= 100.0f; // convert cm → m

    data.bmi = (h > 0) ? weight / (h * h) : 0.0f;

    // BMI classification
    if (data.bmi < 18.5) 
        data.bmi_status = 0;
    else if (data.bmi < 25.0) 
        data.bmi_status = 1;
    else if (data.bmi < 30.0) 
        data.bmi_status = 2;
    else if (data.bmi < 35.0) 
        data.bmi_status = 3;
    else if (data.bmi < 40.0) 
        data.bmi_status = 4;
    else data.bmi_status = 5;

    // BP
    if (bp_sys >= 180 || bp_dias >= 120) 
        data.bp_status = 5;
    else if (bp_sys >= 140 || bp_dias >= 90) 
        data.bp_status = 4;
    else if (bp_sys >= 130 || bp_dias >= 80) 
        data.bp_status = 3;
    else if (bp_sys >= 120 && bp_dias < 80) 
        data.bp_status = 2;
    else if (bp_sys < 120 && bp_dias < 80) 
        data.bp_status = 1;
    else data.bp_status = 0;

    // Blood Sugar
    if (bs < 80)   
        data.bs_status = 0;
    else if (bs < 90) 
        data.bs_status = 1;
    else if (bs < 140) 
        data.bs_status = 2;
    else if (bs < 220) 
        data.bs_status = 3;
    else 
        data.bs_status = 4;

    // Cholesterol
    if (chol < 200) 
        data.chol_status = 0;
    else if (chol < 240) 
        data.chol_status = 1;
    else 
        data.chol_status = 2;

    return data;
}

// -----------------------------------------
// RECOMMENDATIONS FUNCTION
// -----------------------------------------

void dietAddAvoid(HealthData data, FILE *fp) {
    #define PRINT_LINE(fmt, ...) \
        do { \
            if(fp) fprintf(fp, fmt, ##__VA_ARGS__); \
            printf(fmt, ##__VA_ARGS__); \
        } while(0)

    fprintf(fp, "\n==========================================\n");
    fprintf(fp, "            DIET RECOMMENDATIONS\n");
    fprintf(fp, "==========================================\n");

    fprintf(fp, "\n>>> WHAT YOU SHOULD ADD TO YOUR DIET <<<\n");

    // HIGH BP
    if (data.bp_status >= 3) {
        fprintf(fp, "\n[For High Blood Pressure]\n");
        fprintf(fp, "- More potassium-rich fruits (banana, avocado).\n");
        fprintf(fp, "- Vegetables: broccoli, spinach, carrots.\n");
        fprintf(fp, "- Lean protein: chicken breast, fish.\n");
        fprintf(fp, "- Whole grains instead of white rice.\n");
    }

    // LOW BP
    if (data.bp_status == 0) {
        fprintf(fp, "\n[For Low Blood Pressure]\n");
        fprintf(fp, "- Drink more fluids (water, coconut water).\n");
        fprintf(fp, "- Small frequent meals.\n");
        fprintf(fp, "- Moderate salty snacks.\n");
        fprintf(fp, "- Iron-rich foods (spinach, liver).\n");
    }

    // HIGH BLOOD SUGAR
    if (data.bs_status >= 3) {
        fprintf(fp, "\n[For High Blood Sugar]\n");
        fprintf(fp, "- High-fiber vegetables (ampalaya, okra).\n");
        fprintf(fp, "- Brown rice instead of white.\n");
        fprintf(fp, "- Protein foods (egg, tofu, chicken breast).\n");
        fprintf(fp, "- Sugar-free drinks only.\n");
    }

    // LOW BLOOD SUGAR (Dangerously Low or Low)
    if (data.bs_status <= 1) {
        fprintf(fp, "\n[For Low Blood Sugar]\n");
        fprintf(fp, "- Eat every 2–3 hours.\n");
        fprintf(fp, "- Fruits with natural sugar (banana, mango).\n");
        fprintf(fp, "- Milk, yogurt, whole grains.\n");
        fprintf(fp, "- Never skip meals.\n");
    }

    // HIGH CHOLESTEROL
    if (data.chol_status == 2) {
        fprintf(fp, "\n[For High Cholesterol]\n");
        fprintf(fp, "- Oatmeal daily.\n");
        fprintf(fp, "- Fish rich in omega-3 (salmon, sardines).\n");
        fprintf(fp, "- High-fiber fruits.\n");
        fprintf(fp, "- Steamed/boiled vegetables.\n");
    }

    // HIGH BMI
    if (data.bmi_status >= 2) {
        fprintf(fp, "\n[For High BMI]\n");
        fprintf(fp, "- More vegetables, lean protein.\n");
        fprintf(fp, "- Low-calorie snacks.\n");
        fprintf(fp, "- Water instead of sugary drinks.\n");
    }

    // LOW BMI
    if (data.bmi_status == 0) {
        fprintf(fp, "\n[For Low BMI]\n");
        fprintf(fp, "- High-calorie healthy foods.\n");
        fprintf(fp, "- Protein-rich meals.\n");
        fprintf(fp, "- Smoothies with milk.\n");
        fprintf(fp, "- Frequent meals and snacks.\n");
    }

    fprintf(fp, "\n>>> WHAT YOU SHOULD AVOID <<<\n");

    // HIGH BP AVOID
    if (data.bp_status >= 3) {
        fprintf(fp, "\n[For High Blood Pressure]\n");
        fprintf(fp, "- Salty foods.\n");
        fprintf(fp, "- Instant noodles.\n");
        fprintf(fp, "- Processed meats.\n");
        fprintf(fp, "- Soy sauce, patis, salty snacks.\n");
    }

    // LOW BP AVOID
    if (data.bp_status == 0) {
        fprintf(fp, "\n[For Low Blood Pressure]\n");
        fprintf(fp, "- Excessive alcohol.\n");
        fprintf(fp, "- Skipping meals.\n");
        fprintf(fp, "- Heavy meals at once.\n");
    }

    // HIGH SUGAR AVOID
    if (data.bs_status >= 3) {
        fprintf(fp, "\n[For High Blood Sugar]\n");
        fprintf(fp, "- Soda, juice, milk tea.\n");
        fprintf(fp, "- White bread, pastries.\n");
        fprintf(fp, "- Too much rice.\n");
        fprintf(fp, "- Sugary snacks.\n");
    }

    // LOW SUGAR AVOID
    if (data.bs_status <= 1) {
        fprintf(fp, "\n[For Low Blood Sugar]\n");
        fprintf(fp, "- Skipping meals.\n");
        fprintf(fp, "- Too much caffeine.\n");
        fprintf(fp, "- Heavy exercise without food.\n");
    }

    // HIGH CHOLESTEROL AVOID
    if (data.chol_status == 2) {
        fprintf(fp, "\n[For High Cholesterol]\n");
        fprintf(fp, "- Fried foods.\n");
        fprintf(fp, "- Fatty pork and beef.\n");
        fprintf(fp, "- Butter-heavy dishes.\n");
        fprintf(fp, "- Fast food and ice cream.\n");
    }

    fprintf(fp, "\n==========================================\n");
}

void exerciseAddAvoid(HealthData data, FILE *fp) {

    fprintf(fp, "\n==========================================\n");
    fprintf(fp, "          EXERCISE RECOMMENDATIONS\n");
    fprintf(fp, "==========================================\n");

    fprintf(fp, "\n>>> GENERAL EXERCISE TIPS <<<\n");

    // HIGH BP
    if (data.bp_status >= 3) {
        fprintf(fp, "\n[For High Blood Pressure]\n");
        fprintf(fp, "- 30–40 minutes brisk walking daily.\n");
        fprintf(fp, "- Light stretching and breathing exercises.\n");
        fprintf(fp, "- Avoid heavy lifting.\n");
    }

    // LOW BP
    if (data.bp_status == 0) {
        fprintf(fp, "\n[For Low Blood Pressure]\n");
        fprintf(fp, "- Light to moderate movements only.\n");
        fprintf(fp, "- Stay hydrated before exercising.\n");
        fprintf(fp, "- Avoid sudden intense activities.\n");
    }

    // HIGH BLOOD SUGAR
    if (data.bs_status >= 3) {
        fprintf(fp, "\n[For High Blood Sugar]\n");
        fprintf(fp, "- 10–15 min walk after meals.\n");
        fprintf(fp, "- Low-impact cardio: cycling, swimming.\n");
        fprintf(fp, "- Daily stretching.\n");
    }

    // LOW BLOOD SUGAR
    if (data.bs_status <= 1) {
        fprintf(fp, "\n[For Low Blood Sugar]\n");
        fprintf(fp, "- No exercise on empty stomach.\n");
        fprintf(fp, "- Always keep glucose or candy nearby.\n");
        fprintf(fp, "- Light walking or yoga.\n");
    }

    // HIGH CHOLESTEROL
    if (data.chol_status == 2) {
        fprintf(fp, "\n[For High Cholesterol]\n");
        fprintf(fp, "- 40–60 min cardio 3–4x/week.\n");
        fprintf(fp, "- Strength training twice a week.\n");
    }

    // High BMI
    if (data.bmi_status >= 2) {
        fprintf(fp, "\n[For High BMI]\n");
        fprintf(fp, "- 30–45 min cardio daily.\n");
        fprintf(fp, "- Strength training slowly increasing intensity.\n");
    }

    // Low BMI
    if (data.bmi_status == 0) {
        fprintf(fp, "\n[For Low BMI]\n");
        fprintf(fp, "- Focus on muscle-gain exercises.\n");
        fprintf(fp, "- Avoid too much cardio.\n");
        fprintf(fp, "- Moderate weight training.\n");
    }

    fprintf(fp, "\n>>> EXERCISES TO AVOID <<<\n");

    if (data.bp_status >= 3)
        fprintf(fp, "- Heavy lifting, HIIT.\n");

    if (data.bp_status == 0)
        fprintf(fp, "- Sudden intense workouts.\n");

    if (data.bs_status >= 3)
        fprintf(fp, "- Long fasted cardio.\n");

    if (data.bs_status <= 1)
        fprintf(fp, "- Intense workouts without pre-meal.\n");

    if (data.bmi_status >= 2)
        fprintf(fp, "- High-impact intensive jumping workouts.\n");

    if (data.bmi_status == 0)
        fprintf(fp, "- Long cardio sessions.\n");

    fprintf(fp, "\n==========================================\n");
}

// -----------------------------------------
// MAIN MENU
// -----------------------------------------

int main() {
    Profile user;
    int exists = loadProfile(&user);

    while (1) {
        printf("\n=== MY PERSONAL HEALTH TRACKER ===\n");
        if (exists) printf("Active Profile: %s\n", user.name);
        else printf("No Profile Found.\n");

        printf("1. Update/Create Profile\n");
        printf("2. View Full Report\n");
        printf("3. View Diet Recommendations\n");
        printf("4. View Exercise Recommendations\n");
        printf("5. Exit\n");
        printf("Choice: ");

        int choice;
        scanf("%d", &choice);

        if (choice == 1) {

            if (!exists) {
                printf("Enter Name: ");
                scanf(" %49[^\n]", user.name);
            }

            printf("Weight (kg): ");
            scanf("%f", &user.weight);

            printf("Height (m or cm): ");
            scanf("%f", &user.height);

            printf("BP Systolic: ");
            scanf("%d", &user.bp_sys);

            printf("BP Diastolic: ");
            scanf("%d", &user.bp_dias);

            printf("Blood Sugar: ");
            scanf("%d", &user.bs);

            printf("Cholesterol: ");
            scanf("%d", &user.chol);

            user.analysis = analyzeData(
                user.weight, user.height,
                user.bp_sys, user.bp_dias,
                user.bs, user.chol
            );

            saveProfile(user);
            exists = 1;

            printf("\nProfile saved!\n");
        }
        else if (choice == 2) {
            if (!exists)
                printf("No profile exists. Create one first.\n");
            else
                generateReport(user);
        }
        else if (choice == 3) {
            if (!exists) {
                printf("No profile exists. Create one first.\n");
            } else {
                FILE* fp = fopen(report_file, "a");  // append mode
                if (!fp) fp = stdout;
                time_t now = time(NULL);
                struct tm* t = localtime(&now);
                fprintf(fp, "\n\n===== Diet Recommendations generated on %04d-%02d-%02d %02d:%02d:%02d =====\n",
                        t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
                        t->tm_hour, t->tm_min, t->tm_sec);

                dietAddAvoid(user.analysis, fp);
                if (fp != stdout) fclose(fp);
                printf("\nDiet recommendations appended in %s\n", report_file);
            }
        }
        else if (choice == 4) {
            if (!exists) {
                printf("No profile exists. Create one first.\n");
            } else {
                FILE* fp = fopen(report_file, "a");  // append mode
                if (!fp) fp = stdout;
                time_t now = time(NULL);
                struct tm* t = localtime(&now);
                fprintf(fp, "\n\n===== Exercise Recommendations generated on %04d-%02d-%02d %02d:%02d:%02d =====\n",
                        t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
                        t->tm_hour, t->tm_min, t->tm_sec);

                exerciseAddAvoid(user.analysis, fp);
                if (fp != stdout) fclose(fp);
                printf("\nExercise recommendations appended in %s\n", report_file);
            }
        }

        else break;
    }

    return 0;
}
