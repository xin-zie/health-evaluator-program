#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- Constants for File Names ---
#define profile_file "profile.csv"
#define report_file "health_report.txt"

// --- Structure Definitions ---

// HealthData: Stores the calculated BMI and status codes based on the analysis.
typedef struct {
    float bmi;
    int bmi_status; // 0=Underweight, 1=Normal, ..., 5=Obesity Class 3
    int bp_status;  // 0=Hypotension, 1=Normal, ..., 5=Hypertensive Crisis
    int bs_status;  // 0=Dangerously Low, 1=Low, ..., 4=Dangerously High
    int chol_status; // 0=Low Risk, 1=Borderline, 2=High Risk
} HealthData;

// Profile: Stores all user input data and the resulting analysis.
typedef struct {
    char name[50];
    int age;
    float weight;    // In kilograms (kg)
    float height;    // In meters (m) or centimeters (cm) - auto-detected
    int bp_sys;      // Blood Pressure Systolic (mmHg)
    int bp_dias;     // Blood Pressure Diastolic (mmHg)
    int bs;          // Blood Sugar (mg/dL) - float was suggested in main, but analysis uses int for simplicity
    int chol;        // Cholesterol (mg/dL) - float was suggested in main, but analysis uses int for simplicity
    int bs_flag;     // (Not used in the current logic, but left in the struct)
    int chol_type;   // 1=Total, 2=LDL, 3=HDL, 4=Triglycerides
    int hrs;         // Hours since last meal (1=0-2, 2=2-4, 3=4-8)
    HealthData analysis; // The calculated health analysis results
} Profile;

// --- Function Prototypes ---
HealthData analyzeData(float weight, float height, int bp_sys, int bp_dias, int bs, int chol, int chol_type, int hrs); 
void dietAddAvoid(HealthData data, FILE *fp);
void exerciseAddAvoid(HealthData data, FILE *fp);
void saveProfile(Profile p);
int loadProfile(Profile* p);
void generateReport(Profile p);
float get_valid_float(const char *prompt);
int get_valid_int(const char *prompt);

// --- Global Constant Arrays (for Labels) ---

// BMI Status Labels (Index corresponds to bmi_status in HealthData)
const char *const bmi_labels[] = {
    "Underweight",
    "Normal",
    "Overweight",
    "Obesity Class 1",
    "Obesity Class 2",
    "Obesity Class 3"
};

// Blood Pressure Status Labels (Index corresponds to bp_status in HealthData)
const char *const bp_labels[] = {
    "Hypotension (Low)",
    "Normal",
    "Elevated",
    "Stage 1 Hypertension",
    "Stage 2 Hypertension",
    "Hypertensive Crisis"
};

// Blood Sugar Status Labels (Index corresponds to bs_status in HealthData)
const char *const bs_labels[] = {
    "Dangerously Low",
    "Low",
    "Normal",
    "High",
    "Dangerously High"
};

// Cholesterol Status Labels (Index corresponds to chol_status in HealthData)
const char *const chol_labels[] = {
    "Low Heart Disease Risk",
    "Borderline Risk",
    "High Risk"
};

// Time Since Last Meal Labels (Index hrs - 1)
const char *const hrs_labels[] = {
    "0-2 Hours After Meal",
    "2-4 Hours After Meal",
    "4-8 Hours After Meal"
};

// Cholesterol Type Labels (Index chol_type - 1)
const char *const cholType_labels[] = {
    "Total",
    "Low-Density Lipoprotein",
    "High-Density Lipoprotein",
    "Triglycerides"
};


// -----------------------------------------
// SAVE PROFILE TO CSV
// -----------------------------------------
// Saves the user's current profile data to a CSV file.
void saveProfile(Profile p) {
    // Open file in write mode ("w") - overwrites existing data
    FILE* file = fopen(profile_file, "w");
    if (!file) {
        perror("Error opening profile file for writing");
        return;
    }
    // Correcting the format string to match the Profile struct fields:
    fprintf(file, "%s, %d, %.2f, %.2f, %d, %d, %d, %d, %d, %d\n",
            p.name, p.age, p.weight, p.height,
            p.bp_sys, p.bp_dias, p.bs, p.chol, p.chol_type, p.hrs);

    fclose(file);
}

// -----------------------------------------
// LOAD PROFILE FROM CSV
// -----------------------------------------
// Loads the user's profile data from the CSV file.
int loadProfile(Profile* p) {
    // Open file in read mode ("r")
    FILE* file = fopen(profile_file, "r");
    if (!file) return 0; // File does not exist

    // Read the data back from the file. Use the corrected format string.
    // Returns 10 if all 10 items (name, age, weight, height, bp_sys, bp_dias, bs, chol, chol_type, hrs) are read.
    if (fscanf(file, "%49[^,], %d, %f, %f, %d, %d, %d, %d, %d, %d",
               p->name, &p->age, &p->weight, &p->height,
               &p->bp_sys, &p->bp_dias, &p->bs, &p->chol, &p->chol_type, &p->hrs) != 10) {
        fclose(file);
        return 0; // Failed to read all data
    }

    fclose(file);

    // Re-analyze the data immediately upon loading
    p->analysis = analyzeData(
        p->weight, p->height,
        p->bp_sys, p->bp_dias,
        p->bs, p->chol, p->chol_type, p->hrs
    );

    return 1; // Profile loaded successfully
}

// -----------------------------------------
// REPORT GENERATOR
// -----------------------------------------
// Generates a basic health summary report in a text file.
void generateReport(Profile p) {
    // Open the report file in write mode ("w") - overwrites previous report
    FILE* fp = fopen(report_file, "w");
    if (!fp) {
        perror("Error opening report file for writing");
        return;
    }

    // Report Header
    fprintf(fp, "HEALTH REPORT FOR: %s\n", p.name);
    fprintf(fp, "Age: %d\n", p.age);
    fprintf(fp, "==============================\n");

    // BMI Summary
    fprintf(fp, "Weight: %.2f kg, Height: %.2f m\n", p.weight, p.height);
    fprintf(fp, "BMI: %.2f (Status: %s)\n",
            p.analysis.bmi,
            bmi_labels[p.analysis.bmi_status]);

    // Blood Pressure Summary
    fprintf(fp, "Blood Pressure: %d/%d mmHg (%s)\n",
            p.bp_sys, p.bp_dias,
            bp_labels[p.analysis.bp_status]);

    // Blood Sugar Summary
    // Note: hrs-1 is used to get the correct label array index (since hrs is 1-based)
    fprintf(fp, "Blood Sugar (%s): %d mg/dL (%s)\n",
            hrs_labels[p.hrs - 1],
            p.bs,
            bs_labels[p.analysis.bs_status]);

    // Cholesterol Summary
    // Note: chol_type-1 is used to get the correct label array index
    fprintf(fp, "Cholesterol (%s): %d mg/dL (%s)\n",
            cholType_labels[p.chol_type - 1],
            p.chol,
            chol_labels[p.analysis.chol_status]);

    fclose(fp);

    printf("\n[SUCCESS] Personal report generated in %s\n", report_file);
}

// -----------------------------------------
// ANALYSIS FUNCTION
// -----------------------------------------
// Calculates BMI and determines health status classifications.
HealthData analyzeData(float weight, float height, int bp_sys, int bp_dias, int bs, int chol, int chol_type, int hrs) {
    HealthData data;

    // --- BMI Calculation ---
    float h = height;
    // Auto detect height in cm or meters: If height is > 3.0 (i.e., > 300cm), assume cm and convert to meters.
    if (h > 3.0f) h /= 100.0f; // convert cm → m
    // BMI Formula: weight (kg) / height^2 (m^2)
    data.bmi = (h > 0) ? weight / (h * h) : 0.0f; // Check for division by zero

    // BMI Classification (WHO/CDC standard for adults)
    if (data.bmi < 18.5) 
        data.bmi_status = 0; // Underweight
    else if (data.bmi < 25.0) 
        data.bmi_status = 1; // Normal
    else if (data.bmi < 30.0) 
        data.bmi_status = 2; // Overweight
    else if (data.bmi < 35.0) 
        data.bmi_status = 3; // Obesity Class 1
    else if (data.bmi < 40.0) 
        data.bmi_status = 4; // Obesity Class 2
    else
        data.bmi_status = 5; // Obesity Class 3

    // --- Blood Pressure Classification (ACC/AHA 2017) ---
    // Note: The logic correctly uses 'OR' to categorize based on the higher status of either SBP or DBP.
    if (bp_sys >= 180 || bp_dias >= 120) 
        data.bp_status = 5; // Hypertensive Crisis
    else if (bp_sys >= 140 || bp_dias >= 90) 
        data.bp_status = 4; // Stage 2 Hypertension
    else if (bp_sys >= 130 || bp_dias >= 80) 
        data.bp_status = 3; // Stage 1 Hypertension
    else if (bp_sys >= 120 && bp_dias < 80) 
        data.bp_status = 2; // Elevated
    else if (bp_sys < 120 && bp_dias < 80) 
        data.bp_status = 1; // Normal
    else
        data.bp_status = 0; // Hypotension (Low - based on SBP < 90/DBP < 60, but simplified in this code to catch all others)
        
    // --- Blood Sugar Classification (mg/dL) ---
    // Thresholds depend on 'hrs' (time since last meal).
    if (hrs == 1){ // 0-2 Hours After Meal (Postprandial)
        if (bs < 80)   
            data.bs_status = 0; // Dangerously Low
        else if (bs < 90) 
            data.bs_status = 1; // Low
        else if (bs < 140) 
            data.bs_status = 2; // Normal (Typical non-diabetic range)
        else if (bs < 300) 
            data.bs_status = 3; // High
        else 
            data.bs_status = 4; // Dangerously High
    }
    else if (hrs == 2){ // 2-4 Hours After Meal (Still postprandial)
        if (bs < 70)   
            data.bs_status = 0; // Dangerously Low
        else if (bs < 90) 
            data.bs_status = 1; // Low
        else if (bs < 130) 
            data.bs_status = 2; // Normal (Returning to pre-meal levels)
        else if (bs < 220) 
            data.bs_status = 3; // High
        else 
            data.bs_status = 4; // Dangerously High
    }
    else { // hrs == 3 (4-8 Hours After Meal - Approaching Fasting)
        if (bs < 60)   
            data.bs_status = 0; // Dangerously Low
        else if (bs < 80) 
            data.bs_status = 1; // Low
        else if (bs < 120)  // Using 120 mg/dL as high end of "Normal" approaching fasting
            data.bs_status = 2; // Normal (Fasting/Near-Fasting range)
        else if (bs < 180) 
            data.bs_status = 3; // High
        else 
            data.bs_status = 4; // Dangerously High
    }

    // --- Cholesterol Classification (mg/dL) ---
    // Thresholds depend on 'chol_type'.
    if (chol_type == 1){ // Total Cholesterol
        if (chol < 200) 
            data.chol_status = 0; // Low Heart Disease Risk (Desirable)
        else if (chol < 240) 
            data.chol_status = 1; // Borderline Risk
        else 
            data.chol_status = 2; // High Risk
    }
    else if (chol_type == 2){ // Low-Density Lipoprotein (LDL) Cholesterol
        if (chol < 130)  // Desirable for general population
            data.chol_status = 0; // Low Heart Disease Risk
        else if (chol < 160) 
            data.chol_status = 1; // Borderline Risk
        else 
            data.chol_status = 2; // High Risk
    }
    else if (chol_type == 3){ // High-Density Lipoprotein (HDL) Cholesterol (The higher, the better)
        if (chol < 50)  // Below 50 mg/dL is a risk factor for women's
            data.chol_status = 2; // High Risk (Poor)
        else if (chol < 60) 
            data.chol_status = 1; // Borderline Risk
        else // 60 mg/dL and above is considered protective/optimal
            data.chol_status = 0; // Low Heart Disease Risk (Optimal)
    }
    else{ // chol_type == 4: Triglycerides (Near-Fasting/Fasting)
        if (chol < 150) 
            data.chol_status = 0; // Low Heart Disease Risk (Normal)
        else if (chol < 200) 
            data.chol_status = 1; // Borderline Risk (Borderline High)
        else 
            data.chol_status = 2; // High Risk
    }

    return data;
}

// -----------------------------------------
// DIET RECOMMENDATIONS FUNCTION
// -----------------------------------------
// Provides diet advice based on the calculated health statuses.
void dietAddAvoid(HealthData data, FILE *fp) {
    // Prints to both the file and the console (stdout)

    fprintf(fp, "\n==========================================\n");
    fprintf(fp, "            DIET RECOMMENDATIONS\n");
    fprintf(fp, "==========================================\n");

    fprintf(fp, "\n>>> WHAT YOU SHOULD ADD TO YOUR DIET <<<\n");

    // Recommendations are categorized by the status code.

    // HIGH BP (Elevated, Stage 1, Stage 2, Crisis)
    if (data.bp_status >= 3) {
        fprintf(fp, "\n[For High Blood Pressure (DASH Diet focus)]\n");
        fprintf(fp, "- More potassium-rich fruits (banana, avocado) to balance sodium.\n");
        fprintf(fp, "- Vegetables: broccoli, spinach, carrots (high in fiber and minerals).\n");
        fprintf(fp, "- Lean protein: chicken breast, fish (low in saturated fat).\n");
        fprintf(fp, "- Whole grains instead of white rice/refined carbs.\n");
    }

    // LOW BP (Hypotension)
    if (data.bp_status == 0) {
        fprintf(fp, "\n[For Low Blood Pressure]\n");
        fprintf(fp, "- Drink more fluids (water, coconut water) to increase blood volume.\n");
        fprintf(fp, "- Small frequent meals to avoid postprandial hypotension.\n");
        fprintf(fp, "- Moderate salty snacks (check with a doctor).\n");
        fprintf(fp, "- Iron-rich foods (spinach, liver) to prevent anemia-related low BP.\n");
    }

    // HIGH BLOOD SUGAR (High, Dangerously High)
    if (data.bs_status >= 3) {
        fprintf(fp, "\n[For High Blood Sugar]\n");
        fprintf(fp, "- High-fiber, low Glycemic Index vegetables (ampalaya, okra, leafy greens).\n");
        fprintf(fp, "- Brown rice/oats instead of white (lower GI).\n");
        fprintf(fp, "- Protein foods (egg, tofu, chicken breast) to help stabilize blood sugar.\n");
        fprintf(fp, "- Sugar-free drinks only.\n");
    }

    // LOW BLOOD SUGAR (Dangerously Low or Low)
    if (data.bs_status <= 1) {
        fprintf(fp, "\n[For Low Blood Sugar]\n");
        fprintf(fp, "- Eat every 2–3 hours to maintain steady glucose levels.\n");
        fprintf(fp, "- Fruits with natural sugar (banana, mango - quick energy).\n");
        fprintf(fp, "- Milk, yogurt, whole grains (complex carbs for sustained energy).\n");
        fprintf(fp, "- Never skip meals.\n");
    }

    // HIGH CHOLESTEROL (High Risk)
    if (data.chol_status == 2) {
        fprintf(fp, "\n[For High Cholesterol]\n");
        fprintf(fp, "- Oatmeal daily (soluble fiber).\n");
        fprintf(fp, "- Fish rich in omega-3 (salmon, sardines) to lower triglycerides.\n");
        fprintf(fp, "- High-fiber fruits and legumes.\n");
        fprintf(fp, "- Steamed/boiled vegetables.\n");
    }

    // HIGH BMI (Overweight, all Obesity Classes)
    if (data.bmi_status >= 2) {
        fprintf(fp, "\n[For High BMI]\n");
        fprintf(fp, "- More vegetables, lean protein (for satiety and muscle maintenance).\n");
        fprintf(fp, "- Low-calorie snacks (e.g., plain nuts, fruit).\n");
        fprintf(fp, "- Water instead of sugary drinks.\n");
    }

    // LOW BMI (Underweight)
    if (data.bmi_status == 0) {
        fprintf(fp, "\n[For Low BMI]\n");
        fprintf(fp, "- High-calorie healthy foods (e.g., full-fat dairy, nuts, healthy oils).\n");
        fprintf(fp, "- Protein-rich meals (for muscle gain).\n");
        fprintf(fp, "- Smoothies with milk/yogurt/nut butter (calorie-dense and easy to consume).\n");
        fprintf(fp, "- Frequent meals and snacks.\n");
    }

    fprintf(fp, "\n>>> WHAT YOU SHOULD AVOID <<<\n");

    // HIGH BP AVOID
    if (data.bp_status >= 3) {
        fprintf(fp, "\n[For High Blood Pressure]\n");
        fprintf(fp, "- Salty foods, processed snacks.\n");
        fprintf(fp, "- Instant noodles (high sodium).\n");
        fprintf(fp, "- Processed meats (hot dogs, bacon).\n");
        fprintf(fp, "- High-sodium condiments (Soy sauce, patis).\n");
    }

    // LOW BP AVOID
    if (data.bp_status == 0) {
        fprintf(fp, "\n[For Low Blood Pressure]\n");
        fprintf(fp, "- Excessive alcohol (can cause dehydration and lower BP).\n");
        fprintf(fp, "- Skipping meals.\n");
        fprintf(fp, "- Heavy meals at once (can lead to postprandial hypotension).\n");
    }

    // HIGH SUGAR AVOID
    if (data.bs_status >= 3) {
        fprintf(fp, "\n[For High Blood Sugar]\n");
        fprintf(fp, "- Soda, juice, milk tea (high simple sugars).\n");
        fprintf(fp, "- White bread, pastries (high GI, low fiber).\n");
        fprintf(fp, "- Too much refined rice.\n");
        fprintf(fp, "- Sugary snacks.\n");
    }

    // LOW SUGAR AVOID
    if (data.bs_status <= 1) {
        fprintf(fp, "\n[For Low Blood Sugar]\n");
        fprintf(fp, "- Skipping meals (main cause of hypoglycemia).\n");
        fprintf(fp, "- Too much caffeine (can mask symptoms).\n");
        fprintf(fp, "- Heavy exercise without a pre-meal snack.\n");
    }

    // HIGH CHOLESTEROL AVOID
    if (data.chol_status == 2) {
        fprintf(fp, "\n[For High Cholesterol]\n");
        fprintf(fp, "- Fried foods, excessively greasy dishes (trans/saturated fats).\n");
        fprintf(fp, "- Fatty pork and beef cuts.\n");
        fprintf(fp, "- Butter-heavy dishes.\n");
        fprintf(fp, "- Fast food and ice cream.\n");
    }

    fprintf(fp, "\n==========================================\n");
}

// -----------------------------------------
// EXERCISE RECOMMENDATIONS FUNCTION
// -----------------------------------------
// Provides exercise advice based on the calculated health statuses.
void exerciseAddAvoid(HealthData data, FILE *fp) {

    fprintf(fp, "\n==========================================\n");
    fprintf(fp, "          EXERCISE RECOMMENDATIONS\n");
    fprintf(fp, "==========================================\n");

    fprintf(fp, "\n>>> GENERAL EXERCISE TIPS <<<\n");

    // HIGH BP
    if (data.bp_status >= 3) {
        fprintf(fp, "\n[For High Blood Pressure]\n");
        fprintf(fp, "- 30–40 minutes brisk walking daily (aerobic exercise is best for BP).\n");
        fprintf(fp, "- Light stretching and breathing exercises.\n");
        fprintf(fp, "- **Avoid heavy lifting** (can dangerously spike BP).\n");
    }

    // LOW BP
    if (data.bp_status == 0) {
        fprintf(fp, "\n[For Low Blood Pressure]\n");
        fprintf(fp, "- Light to moderate movements only.\n");
        fprintf(fp, "- Stay well hydrated before exercising.\n");
        fprintf(fp, "- Avoid sudden intense activities (can lead to fainting/dizziness).\n");
    }

    // HIGH BLOOD SUGAR
    if (data.bs_status >= 3) {
        fprintf(fp, "\n[For High Blood Sugar]\n");
        fprintf(fp, "- 10–15 min walk after meals (helps uptake glucose).\n");
        fprintf(fp, "- Low-impact cardio: cycling, swimming.\n");
        fprintf(fp, "- Daily stretching.\n");
    }

    // LOW BLOOD SUGAR
    if (data.bs_status <= 1) {
        fprintf(fp, "\n[For Low Blood Sugar]\n");
        fprintf(fp, "- No exercise on empty stomach.\n");
        fprintf(fp, "- Always keep glucose, juice, or candy nearby.\n");
        fprintf(fp, "- Light walking or yoga.\n");
    }

    // HIGH CHOLESTEROL
    if (data.chol_status == 2) {
        fprintf(fp, "\n[For High Cholesterol]\n");
        fprintf(fp, "- 40–60 min cardio 3–4x/week (aerobic exercise helps raise HDL and lower LDL).\n");
        fprintf(fp, "- Strength training twice a week.\n");
    }

    // High BMI
    if (data.bmi_status >= 2) {
        fprintf(fp, "\n[For High BMI]\n");
        fprintf(fp, "- 30–45 min low-impact cardio daily (walking, cycling, swimming).\n");
        fprintf(fp, "- Strength training, slowly increasing intensity (to build muscle and metabolism).\n");
    }

    // Low BMI
    if (data.bmi_status == 0) {
        fprintf(fp, "\n[For Low BMI]\n");
        fprintf(fp, "- Focus on muscle-gain exercises (strength/resistance training).\n");
        fprintf(fp, "- Avoid too much cardio (can burn too many calories).\n");
        fprintf(fp, "- Moderate weight training.\n");
    }

    fprintf(fp, "\n>>> EXERCISES TO AVOID <<<\n");

    if (data.bp_status >= 3)
        fprintf(fp, "- Heavy lifting, High-Intensity Interval Training (HIIT).\n");

    if (data.bp_status == 0)
        fprintf(fp, "- Sudden intense workouts (risk of dizziness/fainting).\n");

    if (data.bs_status >= 3)
        fprintf(fp, "- Long fasted cardio (risk of ketoacidosis for diabetics, or general over-stress).\n");

    if (data.bs_status <= 1)
        fprintf(fp, "- Intense workouts without pre-meal (risk of hypoglycemia).\n");

    if (data.bmi_status >= 2)
        fprintf(fp, "- High-impact intensive jumping workouts (risk of joint injury).\n");

    if (data.bmi_status == 0)
        fprintf(fp, "- Long cardio sessions (hinders weight/muscle gain).\n");

    fprintf(fp, "\n==========================================\n");
}

// -----------------------------------------
// VALIDATION FUNCTIONS
// -----------------------------------------
// Securely reads a floating-point number from standard input.
float get_valid_float(const char *prompt) {
    float value;
    int check;
    char buffer[100];

    while (1) {
        printf("%s", prompt);
        // 1. Read the entire line of input into a buffer
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            continue; 
        }
        // 2. Attempt to scan a float (%f) and note how many characters were read (%n)
        int chars_read = 0;
        check = sscanf(buffer, "%f%n", &value, &chars_read);

        // 3. Check for successful read AND no extra characters
        if (check == 1) {
            char *p = buffer + chars_read;
            // Skip trailing whitespace (spaces, tabs, newlines)
            while (*p != '\0' && (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')) {
                p++;
            }
            // If we reached the end of the string (\0), the input was valid
            if (*p == '\0') {
                return value;
            }
        }
        // 4. If invalid input, print error and loop again
        printf("Invalid Input. Please Enter number only.\n");
    }
}

// Securely reads an integer from standard input.
int get_valid_int(const char *prompt) {
    int value;
    int check;
    char buffer[100]; 

    while (1) {
        printf("%s", prompt);
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            continue; 
        }

        int chars_read = 0;
        // Attempt to scan an integer (%d) and note how many characters were read (%n)
        check = sscanf(buffer, "%d%n", &value, &chars_read);

        if (check == 1) {
            char *p = buffer + chars_read;

            // Skip trailing whitespace
            while (*p != '\0' && (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')) {
                p++;
            }

            // If we reached the end of the string (\0), the input was valid
            if (*p == '\0') {
                return value; 
            }
        }

        printf("Invalid Input. Please Enter a number.\n");
    }
}

// --------------------------------------------------
// MAIN FUNCTION 
// ------------------------------------------------__
int main() {
    Profile user;
    // Attempt to load existing profile
    int exists = loadProfile(&user);
    int choice;

    while (1) {
        printf("\n=== MY PERSONAL HEALTH TRACKER ===\n");
        // Display current profile status
        if (exists) printf("Active Profile: %s\n", user.name);
        else printf("No Profile Found.\n");

        // Main Menu
        printf("1. Update/Create Profile\n");
        printf("2. View Full Report\n");
        printf("3. View Diet Recommendations\n");
        printf("4. View Exercise Recommendations\n");
        printf("5. Exit\n");
         
        // Get user choice, validated as an integer
        choice = get_valid_int("Choice: "); 

        if (choice == 1) {
            // --- Update/Create Profile ---
            if (!exists) {
                // Only ask for name if creating a new profile
                printf("\nEnter Name: ");
                // Safely read the name, including spaces
                if (fgets(user.name, sizeof(user.name), stdin) != NULL) {
                    size_t len = strlen(user.name);
                    // Remove the trailing newline character
                    if (len > 0 && user.name[len - 1] == '\n') {
                        user.name[len - 1] = '\0';
                    }
                }
            }

            // Get all health metrics using validated input functions
            user.age = get_valid_int("Age: ");
            user.weight = get_valid_float("Weight (kg): "); 
            user.height = get_valid_float("Height (m or cm, will auto-detect): ");
             
            user.bp_sys = get_valid_int("BP Systolic (mmHg): "); 
            user.bp_dias = get_valid_int("BP Diastolic (mmHg): "); 
             
            // Blood Sugar Time Menu
            printf("<<< Time Since Last Meal for Blood Sugar Test (mg/dL)\n");
            printf("      1. 0-2 Hours After Meal\n");
            printf("      2. 2-4 Hours After Meal\n");
            printf("      3. 4-8 Hours After Meal\n");
            do {
                user.hrs = get_valid_int("Choice (1-3): ");
                if (user.hrs < 1 || user.hrs > 3) {
                    printf("Invalid choice. Please enter 1, 2, or 3.\n");
                }
            } while (user.hrs < 1 || user.hrs > 3);
             
            // Blood Sugar value
            user.bs = get_valid_float("Blood Sugar (mg/dL): "); 

            // Cholesterol Type Menu
            printf("<<< Type of Cholesterol Tested (mg/dL)\n");
            printf("      1. Total Cholesterol\n");
            printf("      2. Low-Density Lipoprotein (LDL) Cholesterol\n");
            printf("      3. High-Density Lipoprotein (HDL) Cholesterol\n");
            printf("      4. Triglycerides\n");

            do {
                user.chol_type = get_valid_int("Choice (1-4): ");
                if (user.chol_type < 1 || user.chol_type > 4) {
                    printf("Invalid choice. Please enter 1, 2, 3, or 4.\n");
                }
            } while (user.chol_type < 1 || user.chol_type > 4);
             
            // Cholesterol value
            user.chol = get_valid_float("Cholesterol (mg/dL): "); 

            // Re-analyze data and save the profile
            user.analysis = analyzeData(
                user.weight, user.height,
                user.bp_sys, user.bp_dias,
                user.bs, user.chol, user.chol_type,
                user.hrs
            );

            saveProfile(user);
            exists = 1; // Mark profile as existing

            printf("\n ===== Profile saved! =====\n");
        }
        else if (choice == 2) {
            // --- View Full Report ---
            if (!exists)
                printf("No profile exists. Create one first.\n");
            else
                generateReport(user);
        }
        else if (choice == 3) {
            // --- View Diet Recommendations ---
            if (!exists) {
                printf("No profile exists. Create one first.\n");
            } else {
                // Open file in append mode ("a") to add to existing report
                FILE* fp = fopen(report_file, "a");
                if (!fp) {
                    // Fallback to stdout if file cannot be opened (prints to console)
                    fp = stdout;
                    printf("\n[WARNING] Could not open %s. Printing to console instead.\n", report_file);
                }

                dietAddAvoid(user.analysis, fp);

                // Close the file if it was opened successfully
                if (fp != stdout) fclose(fp);
                printf("\n ===== Diet recommendations appended in %s ===== \n", report_file);
            }
        }
        else if (choice == 4) {
            // --- View Exercise Recommendations ---
            if (!exists) {
                printf("No profile exists. Create one first.\n");
            } else {
                // Open file in append mode ("a")
                FILE* fp = fopen(report_file, "a");
                if (!fp) {
                    // Fallback to stdout
                    fp = stdout;
                    printf("\n[WARNING] Could not open %s. Printing to console instead.\n", report_file);
                }

                exerciseAddAvoid(user.analysis, fp);

                if (fp != stdout) fclose(fp);
                printf("\n ===== Exercise recommendations appended in %s ===== \n", report_file);
            }
        }
        else if (choice == 5) {
            // --- Exit Program ---
            printf("\nExiting Health Tracker. Goodbye!\n");
            break;
        }
        else {
            // Handle valid integers outside the menu range (1-5)
            printf("\n ===== Invalid choice. Please enter a number between 1 and 5. ===== \n");
        }
    }

    return 0;
}
