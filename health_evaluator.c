#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Constants
#define max_test 10
#define max_recommendation 10
#define config_file "patient_data.csv"
#define report_file "evaluation_report.txt"

// RYAN PART
typedef struct{
    float bmi; // Raw calculation of the patient's BMI
    int bmi_status; // 0: Underweight, 1: Normal, 2: Overweight, 3: Class 1 Obesity, 4: Class 2 Obesity, 5: Class 3 Obesity
    int bp_status; // 0: Low (Hypotension), 1: Normal, 2: Elevated, 3: Stage 1 Hypertension, 4: Stage 2 Hypertension, 5: Hypertensive Crisis
    int bs_status; // 0: Dangerously Low, 1: Low, 2: Normal, 3: High, 4: Dangereously High
    int chol_status; // 0: Low Heart Disease Risk, 1: Borderline Heart Disease Risk, 2: High Heart Disease Risk
} HealthData; // var name of the structure

HealthData analyzeData(float weight, float height, int bp_sys, int bp_dias, int bs, int chol){ // bp_sys: systolic, bp_dias: diastolic, bs: blood sugar, chol: cholesterol
    HealthData data; // Create the package named "data"

    //Calculate the BMI
    data.bmi = weight / (height * height);
    printf("BMI: %.2f\n", data.bmi);

    // 1. BMI Status
    printf("Weight Type: ");
    if(data.bmi < 18.5){
        data.bmi_status = 0; // Underweight
        printf("Underweight\n");
    }
    else if(data.bmi < 25.0){
        data.bmi_status = 1; // Normal
        printf("Normal\n");
    }
    else if(data.bmi < 30.0){
        data.bmi_status = 2; // Overweight
        printf("Overweight\n");
    }
    else if(data.bmi < 35.0){
        data.bmi_status = 3; // Class 1 Obesity
        printf("Class 1 Obesity\n");
    }
    else if(data.bmi < 40.0){
        data.bmi_status = 4; // Class 2 Obesity
        printf("Class 2 Obesity\n");
    }
    else{
        data.bmi_status = 5; // Class 3 Obesity
        printf("Class 3 Obesity\n");
    }


    // 2. Blood Pressure
    // "If the two numbers fall under different categories, the higher(worse) category applies", so we start at the worst category possible
    printf("Blood Pressure Category: ");
    if(bp_sys >= 180 || bp_dias >= 120){
        data.bp_status = 5; // Hypertensive Crisis
        printf("Hypertensive Crisis\n");
    }
    else if(bp_sys >= 140 || bp_dias >= 90){
        data.bp_status = 4; // Stage 2 Hypertension
        printf("Stage 2 Hypertension\n");
    }
    else if((bp_sys >= 130 && bp_sys <= 139) || (bp_dias >= 80 && bp_dias <= 89)){
        data.bp_status = 3; // Stage 1 Hypertension
        printf("Stage 1 Hypertension\n");
    }
    else if((bp_sys >= 120 && bp_sys <= 129) && bp_dias >= 80){
        data.bp_status = 2; // Elevated
        printf("Elevated\n");
    }
    else if((bp_sys < 120 && bp_sys >= 90) && (bp_dias < 80 && bp_dias >= 60)){
        data.bp_status = 1; // Normal
        printf("Normal\n");
    }
    else{
        data.bp_status = 0; // Low (Hypotension)
        printf("Low (Hypotension)\n");
    }

    // 3. Blood Sugar
    printf("Blood Sugar Level: ");
    if(bs < 80){
        data.bs_status = 0; // Dangerously Low
        printf("Dangerously Low\n");
    }
    else if(bs < 90){
        data.bs_status = 1; // Low
        printf("Low\n");
    }
    else if(bs < 140){
        data.bs_status = 2; // Normal
        printf("Normal\n");
    }
    else if(bs < 220){
        data.bs_status = 3; // High
        printf("High\n");
    }
    else{
        data.bs_status = 4; // Dangerously High
        printf("Dangerously High\n");
    }


    // 4. Cholesterol
    printf("Cholesterol Classification: ");
    if(chol < 200){
        data.chol_status = 0; // Low Heart Disease Risk
        printf("Low Heart Disease Risk\n");
    }
    else if(chol < 240){
        data.chol_status = 1; // Borderline Heart Disease Risk
        printf("Borderline Heart Disease Risk\n");
    }
    else{
        data.chol_status = 2; // High Heart Disease Risk
        printf("High Heart Disease Risk\n");
    }

    return data; //returns the entire package
}

int main(){
    HealthData results = analyzeData(51.5, 1.65, 125, 88, 150, 220);
}

// Nat's PART
void dietAddAvoid(HealthData data, FILE *fp) {

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

