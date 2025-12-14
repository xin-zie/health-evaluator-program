#include <stdio.h>
#include <stdlib.h>
#include <string.h>
  
#define profile_file "profile.csv"
#define report_file "health_report.txt"

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
    int chol_type;
    int hrs;
    HealthData analysis;
} Profile;

// Function prototypes
HealthData analyzeData(float weight, float height, int bp_sys, int bp_dias, int bs, int chol, int chol_type, int hrs); 
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

const char *const hrs_labels[] = {
    "0-2 Hours After Meal",
    "2-4 Hours After Meal",
    "4-8 Hours After Meal"
};

const char *const cholType_labels[] = {
    "Total",
    "Low-Density Lipoprotein",
    "High-Density Lipoprotein",
    "Triglycerides"
};


// -----------------------------------------
// SAVE PROFILE TO CSV
// -----------------------------------------

void saveProfile(Profile p) {
    FILE* file = fopen(profile_file, "w");
    if (!file) return;

    // Modified fprintf to include the full labels
    fprintf(file, "%s,\nWeight: %.2f,\nHeight: %.2f,\nBlood Pressure Systolic: %d,\nBlood Pressure Diastolic: %d,\nBlood Sugar: %d,\nCholesterol: %d,\nCholesterol Type Option: %d,\nBlood Sugar Option: %d",
            p.name, p.weight, p.height, p.bp_sys, p.bp_dias, p.bs, p.chol, p.chol_type, p.hrs);

    fclose(file);
}

// -----------------------------------------
// LOAD PROFILE FROM CSV
// -----------------------------------------

int loadProfile(Profile* p) {
    FILE* file = fopen(profile_file, "r");
    if (!file) return 0;

    fscanf(file, "%49[^,],%f,%f,%d,%d,%d,%d,%d,%d",
           p->name, &p->weight, &p->height,
           &p->bp_sys, &p->bp_dias, &p->bs, &p->chol, &p->chol_type, &p->hrs);

    fclose(file);

    p->analysis = analyzeData(
        p->weight, p->height,
        p->bp_sys, p->bp_dias,
        p->bs, p->chol, p->chol_type, p->hrs
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

    fprintf(fp, "Blood Sugar (%s): %d (%s)\n",
            hrs_labels[p.hrs - 1],
            p.bs,
            bs_labels[p.analysis.bs_status]);

    fprintf(fp, "Cholesterol (%s): %d (%s)\n",
            cholType_labels[p.chol_type - 1],
            p.chol,
            chol_labels[p.analysis.chol_status]);

    fclose(fp);

    printf("\n[SUCCESS] Personal report generated in %s\n", report_file);
}

// -----------------------------------------
// ANALYSIS FUNCTION
// -----------------------------------------

HealthData analyzeData(float weight, float height, int bp_sys, int bp_dias, int bs, int chol, int chol_type, int hrs) {
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
    if (hrs == 1){ // 0-2 Hours After Meal
        if (bs < 80)   
            data.bs_status = 0;
        else if (bs < 90) 
            data.bs_status = 1;
        else if (bs < 140) 
            data.bs_status = 2;
        else if (bs < 300) 
            data.bs_status = 3;
        else 
            data.bs_status = 4;
    }
    else if (hrs == 2){ // 2-4 Hours After Meal
        if (bs < 70)   
            data.bs_status = 0;
        else if (bs < 90) 
            data.bs_status = 1;
        else if (bs < 130) 
            data.bs_status = 2;
        else if (bs < 220) 
            data.bs_status = 3;
        else 
            data.bs_status = 4;
    }
    else { // 4-8 Hours After Meal
        if (bs < 60)   
            data.bs_status = 0;
        else if (bs < 80) 
            data.bs_status = 1;
        else if (bs < 120) 
            data.bs_status = 2;
        else if (bs < 180) 
            data.bs_status = 3;
        else 
            data.bs_status = 4;
    }
    // Cholesterol
    if (chol_type == 1){ // Total Cholesterol
        if (chol < 200) 
            data.chol_status = 0;
        else if (chol < 240) 
            data.chol_status = 1;
        else 
            data.chol_status = 2;
    }
    else if (chol_type == 2){ // Low-Density Lipoprotein (LDL) Cholesterol
        if (chol < 130) 
            data.chol_status = 0;
        else if (chol < 160) 
            data.chol_status = 1;
        else 
            data.chol_status = 2;
    }
    else if (chol_type == 3){ // High-Density Lipoprotein (HDL) Cholesterol
        if (chol < 50) 
            data.chol_status = 2;
        else if (chol < 60) 
            data.chol_status = 1;
        else 
            data.chol_status = 0;
    }
    else{ // Triglycerides
        if (chol < 150) 
            data.chol_status = 0;
        else if (chol < 200) 
            data.chol_status = 1;
        else 
            data.chol_status = 2;
    }

    return data;
}

// -----------------------------------------
// RECOMMENDATIONS FUNCTION
// -----------------------------------------

void dietAddAvoid(HealthData data, FILE *fp) {
  
/* This function analyzes the user's health status (BP, blood sugar,
cholesterol, and BMI) and outputs appropriate diet recommendations
to a file using the given file pointer.*/
  
    fprintf(fp, "\n==========================================\n");
    fprintf(fp, "            DIET RECOMMENDATIONS\n");
    fprintf(fp, "==========================================\n");

    fprintf(fp, "\n>>> WHAT YOU SHOULD ADD TO YOUR DIET <<<\n");

    // HIGH BP: prints recomendations if bp_status is greater than or equal to 3
    if (data.bp_status >= 3) {
        fprintf(fp, "\n[For High Blood Pressure]\n");
        fprintf(fp, "- More potassium-rich fruits (banana, avocado).\n");
        fprintf(fp, "- Vegetables: broccoli, spinach, carrots.\n");
        fprintf(fp, "- Lean protein: chicken breast, fish.\n");
        fprintf(fp, "- Whole grains instead of white rice.\n");
    } //From the article : Foods that lower blood pressure by Victoria Taylor(2024).

    // LOW BP: prints recomendations if bp_status is equal to 0
    if (data.bp_status == 0) {
        fprintf(fp, "\n[For Low Blood Pressure]\n");
        fprintf(fp, "- Drink more fluids (water, coconut water).\n");
        fprintf(fp, "- Small frequent meals.\n");
        fprintf(fp, "- Moderate salty snacks.\n");
        fprintf(fp, "- Foods high in folate (asparagus,liver).\n");
    }//From the article : Raise low blood pressure naturally through diet by Cory Whelan (2025).

    // HIGH BLOOD SUGAR:prints recomendations if bs_status greater than or equal to 3
    if (data.bs_status >= 3) {
        fprintf(fp, "\n[For High Blood Sugar]\n");
        fprintf(fp, "- High-fiber vegetables (ampalaya, okra).\n");
        fprintf(fp, "- Brown rice instead of white.\n");
        fprintf(fp, "- Protein foods (egg, tofu, chicken breast).\n");
        fprintf(fp, "- Nonfat or low-fat dairy (milk, yogurt.\n");
    }//From National Library of Medicine.Diabetic diet.

    // LOW BLOOD SUGAR (Dangerously Low or Low) prints recomendations if bs_status lesser than or equal to 1
    if (data.bs_status <= 1) {
        fprintf(fp, "\n[For Low Blood Sugar]\n");
        fprintf(fp, "- Eat small meals every 3-4 hours.\n");
        fprintf(fp, "- Fruits with natural sugar (banana, mango).\n");
        fprintf(fp, "- Milk, yogurt, whole grains.\n");
        fprintf(fp, "- Never skip meals.\n");
    }//From thr article: A meal plan to help you manage hypoglycemia by Cory Whelan (2025).

    // HIGH CHOLESTEROL:prints recomendations if chol_status is equal to 2
    if (data.chol_status == 2) {
        fprintf(fp, "\n[For High Cholesterol]\n");
        fprintf(fp, "- Plant stanols and sterols(whole grains, nuts).\n");
        fprintf(fp, "- Fish rich in omega-3 (salmon, sardines).\n");
        fprintf(fp, "- High-fiber fruits.\n");
        fprintf(fp, "- Steamed/boiled vegetables.\n");
    }// From National Library of Medicine. How to Lower Cholesterol with Diet. 

    // HIGH BMI:prints recomendations if bmi_status is greater than or equal to 2.
    if (data.bmi_status >= 2) {
        fprintf(fp, "\n[For High BMI]\n");
        fprintf(fp, "- Lean protein(chicken breast,red meats).\n");
        fprintf(fp, "- Cruciferous vegetables(broccoli, cauliflower).\n");
        fprintf(fp, "- Whole grains.\n");
    }//From the article : 16 of the Best Foods for Your Healthy Weight Journey by Lisa Wartenberg(2025)

    // LOW BMI:prints recomendations if bmi_status is equal to 0.
    if (data.bmi_status == 0) {
        fprintf(fp, "\n[For Low BMI]\n");
        fprintf(fp, "- High-calorie healthy foods.\n");
        fprintf(fp, "- Protein-rich meals.\n");
        fprintf(fp, "- Healthy fats(avocados,virgin olive oil). \n");
        fprintf(fp, "- Frequent meals and snacks.\n");
    }//From National Lipid Association. Heart-Healthy eating if you are underweight.

    fprintf(fp, "\n>>> WHAT YOU SHOULD AVOID <<<\n");

    // HIGH BP AVOID :prints recomendations if bp_status is greater than or equal to 3
    if (data.bp_status >= 3) {
        fprintf(fp, "\n[For High Blood Pressure]\n");
        fprintf(fp, "- Salty foods.\n");
        fprintf(fp, "- Sugary and fatty foods.\n");
        fprintf(fp, "- Alcohol.\n");
        fprintf(fp, "- Excess caffeine.\n");
    } //From the article : Foods that lower blood pressure by Victoria Taylor(2024).

    // LOW BP AVOID: prints recomendations if bp_status is equal to 0
    if (data.bp_status == 0) {
        fprintf(fp, "\n[For Low Blood Pressure]\n");
        fprintf(fp, "- Excessive alcohol.\n");
        fprintf(fp, "- Skipping meals.\n");
        fprintf(fp, "- Heavy meals at once.\n");
    }  //From the article : Raise low blood pressure naturally through diet by Cory Whelan (2025).

    // HIGH SUGAR AVOID:prints recomendations if bs_status greater than or equal to 3
    if (data.bs_status >= 3) {
        fprintf(fp, "\n[For High Blood Sugar]\n");
        fprintf(fp, "- High-carb foods and drinks.\n");
        fprintf(fp, "- Fried foods.\n");
        fprintf(fp, "- Foods high in sodium.\n");
        fprintf(fp, "- Alcohol.\n");
    }//From National Library of Medicine.Diabetic diet.

    // LOW SUGAR AVOID:prints recomendations if bs_status lesser than or equal to 1
    if (data.bs_status <= 1) {
        fprintf(fp, "\n[For Low Blood Sugar]\n");
        fprintf(fp, "- Skipping meals.\n");
        fprintf(fp, "- Too much caffeine.\n");
        fprintf(fp, "- Alcohol.\n");
    }//From the article :A meal plan to help you manage hypoglycemia by Cory Whelan (2025).

    // HIGH CHOLESTEROL AVOID:prints recomendations if chol_status is equal to 2
    if (data.chol_status == 2) {
        fprintf(fp, "\n[For High Cholesterol]\n");
        fprintf(fp, "- Fried foods.\n");
        fprintf(fp, "- Fatty pork and beef.\n");
        fprintf(fp, "- Butter-heavy dishes.\n");
        fprintf(fp, "- Salty foods.\n");
    }// From National Library of Medicine. How to Lower Cholesterol with Diet. 
  
    // HIGH BMI AVOID:prints recomendations if bmi_status is greater than or equal to 2.
    if (data.bmi_status >= 2) {
        fprintf(fp, "\n[For High BMI]\n");
        fprintf(fp, "- Sugary drinks.\n");
        fprintf(fp, "- High-calorie foods (french fries,potato chips).\n");
        fprintf(fp, "- Foods high in added sugar (pastries, cookies).\n");
        fprintf(fp, "- Alcohol.\n");
    } //From the article:11 foods to avoid when trying to lose weight by Hrefna Palsdottir(2023)
  
    // LOW BMI AVOID:prints recomendations if bmi_status is equal to 0.
    if (data.bmi_status == 0) {
        fprintf(fp, "\n[For Low BMI]\n");
        fprintf(fp, "- Whole Eggs.\n");
        fprintf(fp, "- Beans and Legumes.\n");
        fprintf(fp, "- Boiled Potatoes.\n");
        fprintf(fp, "- Tuna.\n");
    }//From the article: Diet Chart For underweight Patient by Hirna Firdous(2020).

    fprintf(fp, "\n==========================================\n");
}

void exerciseAddAvoid(HealthData data, FILE *fp) {
  
/* This function analyzes the user's health status (BP, blood sugar,
cholesterol, and BMI) and outputs appropriate exercise recommendations
to a file using the given file pointer.*/
  
    fprintf(fp, "\n==========================================\n");
    fprintf(fp, "          EXERCISE RECOMMENDATIONS\n");
    fprintf(fp, "==========================================\n");

    fprintf(fp, "\n>>> GENERAL EXERCISE TIPS <<<\n");

    // HIGH BP: prints tips if bp_status is greater than or equal to 3
    if (data.bp_status >= 3) {
        fprintf(fp, "\n[For High Blood Pressure]\n");
        fprintf(fp, "- 10 minutes brisk walking daily (aerobic exercise is best for BP).\n");
        fprintf(fp, "- Desk treadmilling or pedal pushing.\n");
        fprintf(fp, "- Swimming.\n");
    }//From the article: The six best exercises to control high blood pressure by Wesley Tyree(2025)

    // LOW BP: prints tips if bp_status is equal to 0
    if (data.bp_status == 0) {
        fprintf(fp, "\n[For Low Blood Pressure]\n");
        fprintf(fp, "- Light to moderate movements only.\n");
        fprintf(fp, "- Stay hydrated before exercising.\n");
        fprintf(fp, "- Monitor symptoms.\n");
    }//From the article: Exercise Tips For People With Low Blood Pressure by Manya Singh(2024)

    // HIGH BLOOD SUGAR:prints tips if bs_status greater than or equal to 3
    if (data.bs_status >= 3) {
        fprintf(fp, "\n[For High Blood Sugar]\n");
        fprintf(fp, "- 15-20 min walk after meals.\n");
        fprintf(fp, "- Low-impact cardio: cycling, swimming.\n");
        fprintf(fp, "- Squats.\n");
        fprintf(fp, "- The soleus push-up.\n");
    }//From the Article: 4 Exercises To Lower Blood Sugar by Paul Heltzel(2024)

    // LOW BLOOD SUGAR (Dangerously Low or Low) prints tips if bs_status lesser than or equal to 1
    if (data.bs_status <= 1) {
        fprintf(fp, "\n[For Low Blood Sugar]\n");
        fprintf(fp, "- No exercise on empty stomach.\n");
        fprintf(fp, "- Always keep glucose or candy nearby.\n");
        fprintf(fp, "- Light walking or yoga.\n");
    }//From the article Food Timing and Exercise With Hypoglycemia by Cara Rosenbloom(2022)

    // HIGH CHOLESTEROL:prints tips if chol_status is equal to 2
    if (data.chol_status == 2) {
        fprintf(fp, "\n[For High Cholesterol]\n");
        fprintf(fp, "- 40–60 min cardio 3–4x/week.\n");
        fprintf(fp, "- Strength training twice a week.\n");
    }// From the Article: Does exercise lower cholesterol? by Adam Rowden(2024)

    // HIGH BMI:prints tips if bmi_status is greater than or equal to 2.
    if (data.bmi_status >= 2) {
        fprintf(fp, "\n[For High BMI]\n");
        fprintf(fp, "- 30–45 min cardio daily.\n");
        fprintf(fp, "- Strength training slowly increasing intensity.\n");
    }//From the article:The Best Exercises for Obese Clients: A Complete guide byPhilip Stefanov (2025)

    // LOW BMI:prints tips if bmi_status is equal to 0.
    if (data.bmi_status == 0) {
        fprintf(fp, "\n[For Low BMI]\n");
        fprintf(fp, "- Focus on muscle-gain exercises(pushups,pullups).\n");
        fprintf(fp, "- Moderate weight training.\n");
    }//From the article:How to exercise to bulk up and shape your body by Tim Jewell

    fprintf(fp, "\n>>> EXERCISES TO AVOID <<<\n");

    // HIGH BP AVOID :prints tips if bp_status is greater than or equal to 3
    if (data.bp_status >= 3)
        fprintf(fp, "- Heavy lifting, HIIT.\n");
    // LOW BP AVOID: prints tips if bp_status is equal to 0
    if (data.bp_status == 0)
        fprintf(fp, "- Sudden intense workouts.\n");
    // HIGH SUGAR AVOID:prints tips if bs_status greater than or equal to 3
    if (data.bs_status >= 3)
        fprintf(fp, "- Long fasted cardio.\n");
    // LOW SUGAR AVOID:prints tips if bs_status lesser than or equal to 1
    if (data.bs_status <= 1)
        fprintf(fp, "- Intense workouts without pre-meal.\n");
    // HIGH BMI AVOID:prints tips if bmi_status is greater than or equal to 2.
    if (data.bmi_status >= 2)
        fprintf(fp, "- High-impact intensive jumping workouts.\n");
     // LOW BMI AVOID:prints tips if bmi_status is equal to 0.
    if (data.bmi_status == 0)
        fprintf(fp, "- Long cardio sessions.\n");

    fprintf(fp, "\n==========================================\n");
}

// -------------------------------
// ERROR HANDLING FUNCTION
// ------------------------------
int get_valid_int(const char *prompt) {
    int value;
    int check;
    char buffer[100]; // Buffer to read the input line

    while (1) {
        // 1. Print the prompt
        printf("%s", prompt);

        // 2. Read the entire line of input into a buffer (safer than plain scanf)
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            // Error reading input
            continue; 
        }

        // 3. Attempt to scan an integer from the buffer
        int chars_read = 0;
        // %d reads the integer. %n stores the number of characters read.
        check = sscanf(buffer, "%d%n", &value, &chars_read);

        // 4. Check the result of sscanf
        if (check == 1) {
            // An integer was successfully read. Now check for extra non-whitespace characters.
            char *p = buffer + chars_read;

            // Check if the rest of the buffer only contains whitespace or newline
            while (*p != '\0' && (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')) {
                p++;
            }

            // If *p is the null terminator, the input was valid
            if (*p == '\0') {
                return value; // Valid whole number entered. Exit the function.
            }
        }

        // 5. If input failed or extra characters were found, print the required error
        printf("Invalid Input. Please Enter a whole number.\n");
        // The loop repeats, asking for input again.
    }
}

// --------------------------------------------------
// MAIN FUNCTION 
// --------------------------------------------------

int main() {
    Profile user;
    int exists = loadProfile(&user);
    int choice;

    while (1) {
        printf("\n=== MY PERSONAL HEALTH TRACKER ===\n");
        if (exists) printf("Active Profile: %s\n", user.name);
        else printf("No Profile Found.\n");

        printf("1. Update/Create Profile\n");
        printf("2. View Full Report\n");
        printf("3. View Diet Recommendations\n");
        printf("4. View Exercise Recommendations\n");
        printf("5. Exit\n");
        
        choice = get_valid_int("Choice: ");

        if (choice == 1) {

            if (!exists) {
                printf("\nEnter Name: ");
                if (fgets(user.name, sizeof(user.name), stdin) != NULL) {
                    size_t len = strlen(user.name);
                    if (len > 0 && user.name[len - 1] == '\n') {
                        user.name[len - 1] = '\0'; // Remove newline
                    }
                }
            }

            user.weight = get_valid_int("Weight (kg): ");
            user.height = get_valid_int("Height (m or cm): ");
            
            user.bp_sys = get_valid_int("BP Systolic: ");
            user.bp_dias = get_valid_int("BP Diastolic: ");
            
            printf("<<< Time Since Last Meal for Blood Sugar Test\n");
            printf("      1. 0-2 Hours After Meal\n");
            printf("      2. 2-4 Hours After Meal\n");
            printf("      3. 4-8 Hours After Meal\n");

            do {
                user.hrs = get_valid_int("Choice: "); // Input 1, 2, or 3 expected
                if (user.hrs > 3) {
                    printf("Invalid choice. Please enter 1, 2, or 3.\n");
                }
            } while (user.hrs > 3);
            user.bs = get_valid_int("Blood Sugar: ");

            printf("<<< Type of Cholesterol Tested\n");
            printf("      1. Total Cholesterol\n");
            printf("      2. Low-Density Lipoprotein (LDL) Cholesterol\n");
            printf("      3. High-Density Lipoprotein (HDL) Cholesterol\n");
            printf("      4. Triglycerides\n");

            do {
                user.chol_type = get_valid_int("Choice: "); // Input 1, 2, 3, or 4 expected
                if (user.chol_type > 4) {
                    printf("Invalid choice. Please enter 1, 2, 3, or 4.\n");
                }
            } while (user.chol_type > 4);
            user.chol = get_valid_int("Cholesterol: ");

            user.analysis = analyzeData(
                user.weight, user.height,
                user.bp_sys, user.bp_dias,
                user.bs, user.chol, user.chol_type,
                user.hrs
            );

            saveProfile(user);
            exists = 1;

            printf("\n ===== Profile saved! =====\n");
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
                FILE* fp = fopen(report_file, "a");
                if (!fp) fp = stdout;

                dietAddAvoid(user.analysis, fp);
                if (fp != stdout) fclose(fp);
                printf("\n ===== Diet recommendations appended in %s ===== \n", report_file);
            }
        }
        else if (choice == 4) {
            if (!exists) {
                printf("No profile exists. Create one first.\n");
            } else {
                FILE* fp = fopen(report_file, "a");
                if (!fp) fp = stdout;

                exerciseAddAvoid(user.analysis, fp);
                if (fp != stdout) fclose(fp);
                printf("\n ===== Exercise recommendations appended in %s ===== \n", report_file);
            }
        }
        else if (choice == 5) {
            break;
        }
        else {
            // This case handles valid integers outside the 1-5 range
            printf("\n ===== Invalid choice. Please enter a number between 1 and 5. ===== \n");
        }
    }

    return 0;
}




