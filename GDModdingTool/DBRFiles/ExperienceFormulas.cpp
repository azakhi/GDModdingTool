#include "ExperienceFormulas.h"

void ExperienceFormulas::parse() {
    if (_isParsed) {
        return;
    }

    DBRBase::parse();

    _isParsed = true;
}

void ExperienceFormulas::adjustExpGained(DifficultyType difficulty, float multiplier) {
    switch (difficulty) {
        case Normal:
            adjustFormulaField("experienceEquation", multiplier);
            break;
        case Elite:
            adjustFormulaField("eliteExperienceEquation", multiplier);
            break;
        case Ultimate:
            adjustFormulaField("ultimateExperienceEquation", multiplier);
            break;
        case Challenge:
            adjustFormulaField("challengeExperienceEquation", multiplier);
            break;
        case AllDifficulties:
            adjustFormulaField("experienceEquation", multiplier);
            adjustFormulaField("eliteExperienceEquation", multiplier);
            adjustFormulaField("ultimateExperienceEquation", multiplier);
            adjustFormulaField("challengeExperienceEquation", multiplier);
            break;
        default:
            throw std::runtime_error("Unexpected Difficulty Type");
    }
}

void ExperienceFormulas::setExpGainedEquation(DifficultyType difficulty, std::string value) {
    switch (difficulty) {
        case Normal:
            setFormulaField("experienceEquation", value);
            break;
        case Elite:
            setFormulaField("eliteExperienceEquation", value);
            break;
        case Ultimate:
            setFormulaField("ultimateExperienceEquation", value);
            break;
        case Challenge:
            setFormulaField("challengeExperienceEquation", value);
            break;
        case AllDifficulties:
            setFormulaField("experienceEquation", value);
            setFormulaField("eliteExperienceEquation", value);
            setFormulaField("ultimateExperienceEquation", value);
            setFormulaField("challengeExperienceEquation", value);
            break;
        default:
            throw std::runtime_error("Unexpected Difficulty Type");
    }
}

void ExperienceFormulas::adjustDeathPenalty(float multiplier) {
    adjustFormulaField("deathPenaltyEquation", multiplier);
}

void ExperienceFormulas::setDeathPenaltyEquation(std::string value) {
    setFormulaField("deathPenaltyEquation", value);
}