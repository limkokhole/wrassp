#include "wrassp.h"
#include <asspana.h>
#include <dataobj.h>
#include <asspfio.h>
#include <asspmess.h>
#include <spectra.h>
#include <asspdsp.h>
#include <assp.h>
#include <stdlib.h>
#include <ipds_lbl.h>
#include <diff.h>               /* shouldn't asspana include diff.h? */
#include <filter.h>
#include <ksv.h>
#include <ctype.h>              /* tolower() */

#include <R_ext/PrtUtil.h>
W_GENDER        gender[] = {
    {"female", TG_FEMALE, 'f'}
    ,
    /*
     * {"FEMALE", TG_FEMALE, 'f'} 
     */
    /*
     * , 
     */
    {"male", TG_MALE, 'm'}
    ,
    /*
     * {"MALE", TG_MALE, 'm'} 
     */
    /*
     * , 
     */
    {"unknown", TG_UNKNOWN, 'u'}
    ,
    /*
     * {"UNKNOWN", TG_UNKNOWN, 'u'} 
     */
    /*
     * , 
     */
    {NULL, TG_NONE, '0'}
};


W_OPT           acfanaOptions[] = {
    {"BeginTime", WO_BEGINTIME}
    ,
    {"CenterTime", WO_CENTRETIME}
    ,
    {"EndTime", WO_ENDTIME}
    ,
    {"WindowShift", WO_MSSHIFT}
    ,
    {"WindowSize", WO_MSSIZE}
    ,
    {"EffectiveLength", WO_MSEFFLEN}
    ,
    {"ExplicitExt", WO_OUTPUTEXT}
    ,                           // DON'T FORGET EXTENSION!!!
    {"Window", WO_WINFUNC}
    ,
    {"AnalysisOrder", WO_ORDER}
    ,
    {"EnergyNormalization", WO_ENERGYNORM}
    ,
    {"LengthNormalization", WO_LENGTHNORM}
    ,
    {"ToFile", WO_TOFILE}
    ,
    {"OutputDirectory", WO_OUTPUTDIR}
    ,
    {"ProgressBar", WO_PBAR}
    ,
    {NULL, WO_NONE}
};

W_OPT           afdiffOptions[] = {
    {"ComputeBackwardDifference", WO_DIFF_OPT_BACKWARD}
    ,
    {"ComputeCentralDifference", WO_DIFF_OPT_CENTRAL}
    ,
    {"Channel", WO_CHANNEL}
    ,
    {"ExplicitExt", WO_OUTPUTEXT}
    ,                           // DON'T FORGET EXTENSION!!!
    {"ToFile", WO_TOFILE}
    ,
    {"OutputDirectory", WO_OUTPUTDIR}
    ,
    {"ProgressBar", WO_PBAR}
    ,
    {NULL, WO_NONE}
};

W_OPT           affilterOptions[] = {
    {"HighPass", WO_HPCUTOFF}
    ,
    {"LowPass", WO_LPCUTOFF}
    ,
    {"StopBand", WO_STOPDB}
    ,
    {"Transition", WO_TBWIDTH}
    ,
    {"UseIIR", WO_USEIIR}
    ,
    {"NumIIRsections", WO_NUMIIRSECS}
    ,
    {"ExplicitExt", WO_OUTPUTEXT}
    ,                           // DON'T FORGET EXTENSION!!!
    {"ProgressBar", WO_PBAR}
    ,
    {"ToFile", WO_TOFILE}
    ,
    {"OutputDirectory", WO_OUTPUTDIR}
    ,
    /*
     * {"-channel"}, 
     */
    /*
     * {"-noDither"}, 
     */
    /*
     * {"-gain"}, 
     */
    {NULL, WO_NONE}
};

W_OPT           f0_ksvOptions[] = {
    {"BeginTime", WO_BEGINTIME}
    ,
    {"EndTime", WO_ENDTIME}
    ,
    {"WindowShift", WO_MSSHIFT}
    ,
    {"Gender", WO_GENDER}
    ,
    {"MaxF", WO_MAXF}
    ,
    {"MinF", WO_MINF}
    ,
    {"MinAmp", WO_VOIMAG}
    ,
    {"MaxZCR", WO_VOIZCR}
    ,
    {"ProgressBar", WO_PBAR}
    ,
    {"ExplicitExt", WO_OUTPUTEXT}
    ,                           // DON'T FORGET EXTENSION!!!
    {"ToFile", WO_TOFILE}
    ,
    {"OutputDirectory", WO_OUTPUTDIR}
    ,
    {NULL, WO_NONE}
};

W_OPT           f0_mhsOptions[] = {
    {"BeginTime", WO_BEGINTIME}
    ,
    {"CenterTime", WO_CENTRETIME}
    ,
    {"EndTime", WO_ENDTIME}
    ,
    {"WindowShift", WO_MSSHIFT}
    ,
    {"Gender", WO_GENDER}
    ,
    {"MaxF", WO_MAXF}
    ,
    {"MinF", WO_MINF}
    ,
    {"MinAmp", WO_VOIMAG}
    ,
    {"MinAC1", WO_VOIAC1PP}
    ,
    {"MinRMS", WO_VOIRMS}
    ,
    {"MaxZCR", WO_VOIZCR}
    ,
    {"MinProb", WO_VOIPROB}
    ,
    {"ProgressBar", WO_PBAR}
    ,
    {"PlainSpectrum", WO_MHS_OPT_POWER}
    ,
    {"ExplicitExt", WO_OUTPUTEXT}
    ,                           // DON'T FORGET EXTENSION!!!
    {"ToFile", WO_TOFILE}
    ,
    {"OutputDirectory", WO_OUTPUTDIR}
    ,
    {NULL, WO_NONE}
};

/**
 * Options array for forest (formant estimation)
 *
 */
W_OPT           forestOptions[] = {
    {"BeginTime", WO_BEGINTIME}
    ,
    {"EndTime", WO_ENDTIME}
    ,
    {"WindowShift", WO_MSSHIFT}
    ,
    {"WindowSize", WO_MSSIZE}
    ,
    {"EffectiveLength", WO_MSEFFLEN}
    ,
    {"NominalF1", WO_NOMF1}
    ,
    {"Gender", WO_GENDER}
    ,
    {"Estimate", WO_INS_EST}
    ,
    {"Order", WO_ORDER}
    ,
    {"IncrOrder", WO_INCREMENT}
    ,
    {"NumFormants", WO_NUMFORMANTS}
    ,
    /*
     * {}, 
     */
    {"Window", WO_WINFUNC}
    ,
    {"Preemphasis", WO_PREEMPH}
    ,
    {"ExplicitExt", WO_OUTPUTEXT}
    ,                           // DON'T FORGET EXTENSION!!!
    {"ProgressBar", WO_PBAR}
    ,
    {"ToFile", WO_TOFILE}
    ,
    {"OutputDirectory", WO_OUTPUTDIR}
    ,
    {NULL, WO_NONE}
};


W_OPT           rfcanaOptions[] = {
    {"BeginTime", WO_BEGINTIME}
    ,
    {"CenterTime", WO_CENTRETIME}
    ,
    {"EndTime", WO_ENDTIME}
    ,
    {"WindowShift", WO_MSSHIFT}
    ,
    {"WindowSize", WO_MSSIZE}
    ,
    {"EffectiveLength", WO_MSEFFLEN}
    ,
    {"ExplicitExt", WO_OUTPUTEXT}
    ,                           // DON'T FORGET EXTENSION!!!
    {"Window", WO_WINFUNC}
    ,
    {"Order", WO_ORDER}
    ,
    {"Preemphasis", WO_PREEMPH}
    ,
    {"LpType", WO_TYPE}
    ,
    {"ProgressBar", WO_PBAR}
    ,
    {"ToFile", WO_TOFILE}
    ,
    {"OutputDirectory", WO_OUTPUTDIR}
    ,
    {NULL, WO_NONE}
};

W_OPT           rmsanaOptions[] = {
    {"BeginTime", WO_BEGINTIME}
    ,
    {"CenterTime", WO_CENTRETIME}
    ,
    {"EndTime", WO_ENDTIME}
    ,
    {"WindowShift", WO_MSSHIFT}
    ,
    {"WindowSize", WO_MSSIZE}
    ,
    {"EffectiveLength", WO_MSEFFLEN}
    ,
    {"Linear", WO_RMS_OPT_LINEAR}
    ,
    {"ExplicitExt", WO_OUTPUTEXT}
    ,                           // DON'T FORGET EXTENSION!!!
    {"Window", WO_WINFUNC}
    ,
    {"ProgressBar", WO_PBAR}
    ,
    {"ToFile", WO_TOFILE}
    ,
    {"OutputDirectory", WO_OUTPUTDIR}
    ,
    {NULL, WO_NONE}
};

W_OPT           spectrumOptions[] = {
    {"BeginTime", WO_BEGINTIME}
    ,
    {"CenterTime", WO_CENTRETIME}
    ,
    {"EndTime", WO_ENDTIME}
    ,
    {"SpectrumType", WO_TYPE}
    ,
    {"Resolution", WO_RESOLUTION}
    ,
    {"FftLength", WO_FFTLEN}
    ,
    {"WindowSize", WO_MSSIZE}
    ,
    {"WindowShift", WO_MSSHIFT}
    ,
    {"Window", WO_WINFUNC}
    ,
    /*
     * DFT spectrum 
     */
    {"Bandwidth", WO_BANDWIDTH}
    ,
    /*
     * LP smoothed spectrum 
     */
    {"EffectiveLength", WO_MSEFFLEN}
    ,
    {"Order", WO_ORDER}
    ,
    {"Preemphasis", WO_PREEMPH}
    ,
    {"Deemphasize", WO_LPS_OPT_DEEMPH}
    ,
    /*
     * Cepstrally smoothed spectrum 
     */
    {"NumCeps", WO_ORDER}
    ,
    /*
     * general stuff 
     */
    {"ExplicitExt", WO_OUTPUTEXT}
    ,                           // DON'T FORGET EXTENSION!!!
    {"ProgressBar", WO_PBAR}
    ,
    {"ToFile", WO_TOFILE}
    ,
    {"OutputDirectory", WO_OUTPUTDIR}
    ,
    {NULL, WO_NONE}
};

W_OPT           zcranaOptions[] = {
    {"BeginTime", WO_BEGINTIME}
    ,
    {"CenterTime", WO_CENTRETIME}
    ,
    {"EndTime", WO_ENDTIME}
    ,
    {"WindowShift", WO_MSSHIFT}
    ,
    {"WindowSize", WO_MSSIZE}
    ,
    {"ExplicitExt", WO_OUTPUTEXT}
    ,                           // DON'T FORGET EXTENSION!!!
    /*
     * {"Window", WO_WINFUNC} 
     */
    /*
     * , 
     */
    {"ProgressBar", WO_PBAR}
    ,
    {"ToFile", WO_TOFILE}
    ,
    {"OutputDirectory", WO_OUTPUTDIR}
    ,
    {NULL, WO_NONE}
};

A_F_LIST        funclist[] = {
    {"acfana", setACFdefaults, computeACF, acfanaOptions, ACF_MAJOR,
     ACF_MINOR,
     ACF_DEF_SUFFIX, AF_ACFANA}
    ,
    {"afdiff", setDiffDefaults, diffSignal, afdiffOptions, DIFF_MAJOR,
     DIFF_MINOR, "AUTO", AF_AFDIFF}
    ,
    {"affilter", setFILTdefaults, computeFilter, affilterOptions,
     FILT_MAJOR, FILT_MINOR,
     "", AF_AFFILTER}
    ,
    {"f0ana", setKSVdefaults, computeF0, f0_ksvOptions, KSV_MAJOR,
     KSV_MINOR,
     KSV_DEF_SUFFIX, AF_KSV_PITCH}
    ,
    {"forest", setFMTdefaults, computeFMT, forestOptions, FMT_MAJOR,
     FMT_MINOR,
     FMT_DEF_SUFFIX, AF_FOREST}
    ,
    {"mhspitch", setMHSdefaults, computeMHS, f0_mhsOptions, MHS_MAJOR,
     MHS_MINOR,
     MHS_DEF_SUFFIX, AF_MHS_PITCH}
    ,
    {"rfcana", setLPdefaults, computeLP, rfcanaOptions, RFC_MAJOR,
     RFC_MINOR,
     "", AF_RFCANA}
    ,
    {"rmsana", setRMSdefaults, computeRMS, rmsanaOptions, RMS_MAJOR,
     RMS_MINOR,
     RMS_DEF_SUFFIX, AF_RMSANA}
    ,
    {"spectrum", setSPECTdefaults, computeSPECT, spectrumOptions,
     SPECT_MAJOR, SPECT_MINOR,
     "", AF_SPECTRUM}
    ,
    {"zcrana", setZCRdefaults, computeZCR, zcranaOptions, ZCR_MAJOR,
     ZCR_MINOR,
     ZCR_DEF_SUFFIX, AF_ZCRANA}
    ,
    {NULL, NULL, NULL, 0, 0, AF_NONE}
};

SEXP
performAssp(SEXP args)
{
    SEXP            el,
                    inputs,
                    res,
                    pBar = R_NilValue,
        utilsPackage,
        newVal;
    const char     *name;
    AOPTS           OPTS;
    AOPTS          *opt = &OPTS;
    W_OPT          *wrasspOptions;
    A_F_LIST       *anaFunc = funclist;
    int             tmp,
                    expExt = 0,
        toFile = 1,
        i = 0;
    char            ext[SUFF_MAX + 1] = "",
        *cPtr = NULL;
    W_GENDER       *gend = NULL;
    WFLIST         *wPtr = NULL;
    LP_TYPE        *lPtr = NULL;
    SPECT_TYPE     *sPtr = NULL;
    DOBJ           *inPtr,
                   *outPtr;
    char           *dPath,
                   *bPath,
                   *oExt,
                    outName[PATH_MAX + 1],
                   *outDir = NULL;

    args = CDR(args);           // skip function name

    /*
     * First Element is file name or vector of file names 
     */
    inputs = CAR(args);
    args = CDR(args);

    /*
     * Second element must be assp function name 
     */
    name = isNull(TAG(args)) ? "" : CHAR(PRINTNAME(TAG(args)));
    if (strcmp(name, "fname") != 0)
        error("Second argument must be named 'fname'");

    el = CAR(args);
    while (anaFunc->funcNum != AF_NONE) {
        if (strcmp(CHAR(STRING_ELT(el, 0)), anaFunc->fName) == 0)
            break;
        anaFunc++;
    }
    if (anaFunc->funcNum == AF_NONE)
        error("Invalid analysis function in performAssp.c");
    if ((anaFunc->setFunc) (opt) == -1)
        error("%d\t$%s\n", asspMsgNum, getAsspMsg(asspMsgNum));

    args = CDR(args);



    for (int i = 0; args != R_NilValue; i++, args = CDR(args)) {
        name = isNull(TAG(args)) ? "" : CHAR(PRINTNAME(TAG(args)));
        wrasspOptions = anaFunc->options;
        while (wrasspOptions->name != NULL) {
            if (strcmp(wrasspOptions->name, name) == 0)
                break;
            wrasspOptions++;
        }
        if (wrasspOptions->name == NULL)
            error("Invalid option %s for ASSP analysis %s.", name,
                  anaFunc->fName);

        el = CAR(args);
        switch (wrasspOptions->optNum) {
        case WO_BEGINTIME:
            opt->beginTime = REAL(el)[0];
            break;
        case WO_ENDTIME:
            opt->endTime = REAL(el)[0];
            break;
        case WO_CENTRETIME:
            if (INTEGER(el)[0]) {
                opt->options |= AOPT_USE_CTIME;
            } else {
                opt->options &= ~AOPT_USE_CTIME;
            }
            break;

        case WO_MSEFFLEN:
            if (INTEGER(el)[0]) {
                opt->options |= AOPT_EFFECTIVE;
                switch (anaFunc->funcNum) {
                case AF_SPECTRUM:
                    opt->options &= ~AOPT_USE_ENBW;
                    break;
                default:
                    // do nothing
                    break;
                }
            } else {
                opt->options &= ~AOPT_EFFECTIVE;
                switch (anaFunc->funcNum) {
                case AF_FOREST:
                    opt->gender = 'u';
                    break;
                default:
                    break;
                }
            }
            break;
        case WO_MSSIZE:
            opt->msSize = REAL(el)[0];
            switch (anaFunc->funcNum) {
            case AF_FOREST:
                switch (opt->gender) {
                case 'f':
                    if (opt->msSize != FMT_DEF_EFFLENf)
                        opt->gender = 'u';
                    break;
                case 'm':
                    if (opt->msSize != FMT_DEF_EFFLENm)
                        opt->gender = 'u';
                    break;
                default:
                    break;
                }
                break;
            case AF_SPECTRUM:
                opt->options &= ~AOPT_USE_ENBW;
                break;
            default:
                // do nothing
                break;
            }
            break;
        case WO_MSSHIFT:
            opt->msShift = REAL(el)[0];
            break;
        case WO_MSSMOOTH:
            opt->msSmooth = REAL(el)[0];
            break;
        case WO_BANDWIDTH:
            opt->bandwidth = REAL(el)[0];
            break;
        case WO_RESOLUTION:
            opt->resolution = REAL(el)[0];
            break;
        case WO_GAIN:
            opt->gain = REAL(el)[0];
            break;
        case WO_RANGE:
            opt->range = REAL(el)[0];
            break;
        case WO_PREEMPH:
            opt->preEmph = REAL(el)[0];
            break;
        case WO_FFTLEN:
            opt->FFTLen = INTEGER(el)[0];
            break;
        case WO_CHANNEL:
            opt->channel = INTEGER(el)[0];
            break;
        case WO_GENDER:
            /*
             * some things need to be set here:
             * for f0_ksv: maxf, minf
             * for f0_mhs: maxf, minf
             * for forest: eff. window length, nominal F1
             */
            gend = gender;
            while (gend->ident != NULL) {
                if (strncmp(gend->ident, CHAR(STRING_ELT(el, 0)), 1) == 0)
                    break;
                gend++;
            }
            if (gend->ident == NULL)
                error("Invalid gender specification %s.",
                      CHAR(STRING_ELT(el, 0)));

            switch (anaFunc->funcNum) {
            case AF_KSV_PITCH:
                tmp = setKSVgenderDefaults(opt, gend->code);
                break;
            case AF_MHS_PITCH:
                tmp = setMHSgenderDefaults(opt, gend->code);
                break;
            case AF_FOREST:
                if (gend->num == TG_UNKNOWN) {
                    opt->gender = tolower((int) gend->code);
                    tmp = 1;
                } else
                    tmp = setFMTgenderDefaults(opt, gend->code);
                break;
            default:
                tmp = 1;
                break;
            }
            if (tmp < 0)
                error("%s", applMessage);
            break;
        case WO_MHS_OPT_POWER:
            if (INTEGER(el)[0])
                opt->options |= MHS_OPT_POWER;
            else
                opt->options &= ~MHS_OPT_POWER;
            break;
        case WO_ORDER:
            tmp = opt->order;
            opt->order = INTEGER(el)[0];
            if (anaFunc->funcNum == AF_FOREST) {
                if ((opt->order % 2) != 0) {
                    opt->order = tmp;
                    error("Prediction order must be an even number.");
                } else {
                    opt->options |= FMT_OPT_LPO_FIXED;
                    opt->increment = 0;
                }
            }
            // return TCL_OK;
            break;
        case WO_INCREMENT:
            opt->increment = INTEGER(el)[0];
            if (anaFunc->funcNum == AF_FOREST) {
                opt->options &= ~FMT_OPT_LPO_FIXED;
                opt->order = 0;
            }
            break;
        case WO_NUMLEVELS:
            opt->numLevels = INTEGER(el)[0];
            break;
        case WO_NUMFORMANTS:
            opt->numFormants = INTEGER(el)[0];
            break;
        case WO_PRECISION:
            opt->precision = INTEGER(el)[0];
            break;
        case WO_ACCURACY:
            opt->accuracy = INTEGER(el)[0];
            break;
        case WO_ALPHA:
            opt->alpha = REAL(el)[0];
            break;
        case WO_THRESHOLD:
            opt->threshold = REAL(el)[0];
            break;
        case WO_MAXF:
            opt->maxF = REAL(el)[0];
            switch (anaFunc->funcNum) {
            case AF_KSV_PITCH:
            case AF_MHS_PITCH:
                opt->gender = 'u';
                break;
            default:
                // do nothing
                break;
            }
            break;
        case WO_MINF:
            opt->minF = REAL(el)[0];
            switch (anaFunc->funcNum) {
            case AF_KSV_PITCH:
            case AF_MHS_PITCH:
                opt->gender = 'u';
                break;
            default:
                // do nothing
                break;
            }
            break;
        case WO_NOMF1:         /* e.g. for formant analysis */
            opt->nomF1 = REAL(el)[0];
            switch (anaFunc->funcNum) {
            case AF_FOREST:
                switch (opt->gender) {
                case 'f':
                    if (opt->nomF1 != FMT_DEF_NOMF1f)
                        opt->gender = 'u';
                    break;
                case 'm':
                    if (opt->nomF1 != FMT_DEF_NOMF1m)
                        opt->gender = 'u';
                    break;
                default:
                    break;
                }
                break;
            default:
                // do nothing
                break;
            }
            break;
        case WO_INS_EST:
            if (INTEGER(el)[0])
                opt->options |= FMT_OPT_INS_ESTS;
            else
                opt->options &= ~FMT_OPT_INS_ESTS;
            break;
        case WO_VOIAC1PP:      /* VOICING thresholds */
            opt->voiAC1 = REAL(el)[0];
            break;
        case WO_VOIMAG:
            opt->voiMag = REAL(el)[0];
            break;
        case WO_VOIPROB:
            opt->voiProb = REAL(el)[0];
            break;
        case WO_VOIRMS:
            opt->voiRMS = REAL(el)[0];
            break;
        case WO_VOIZCR:
            opt->voiZCR = REAL(el)[0];
            break;
        case WO_HPCUTOFF:      /* filter parameters */
            opt->hpCutOff = REAL(el)[0];
            if (expExt == 0)
                tmp = getFILTtype(opt, anaFunc->defExt);
            break;
        case WO_LPCUTOFF:
            opt->lpCutOff = REAL(el)[0];
            if (expExt == 0)
                tmp = getFILTtype(opt, anaFunc->defExt);
            break;
        case WO_STOPDB:
            opt->stopDB = REAL(el)[0];
            if (expExt == 0)
                tmp = getFILTtype(opt, anaFunc->defExt);
            break;
        case WO_TBWIDTH:
            opt->tbWidth = REAL(el)[0];
            if (expExt == 0)
                tmp = getFILTtype(opt, anaFunc->defExt);
            break;
        case WO_USEIIR:
            if (INTEGER(el)[0])
                opt->options |= FILT_OPT_USE_IIR;
            else
                opt->options &= ~FILT_OPT_USE_IIR;
            if (expExt == 0)
                tmp = getFILTtype(opt, anaFunc->defExt);
            break;
        case WO_NUMIIRSECS:
            opt->order = INTEGER(el)[0];
            if (opt->order < 1) {
                error
                    ("Bad value for option -numIIRsections (%i), must be greater 0 (default 4).",
                     opt->order);
                opt->order = FILT_DEF_SECTS;
            }
            break;
        case WO_TYPE:          /* hold-all */
            switch (anaFunc->funcNum) {
            case AF_RFCANA:
                lPtr = lpType;
                while (lPtr->ident != NULL) {
                    if (strcmp(lPtr->ident, CHAR(STRING_ELT(el, 0))) == 0)
                        break;
                    lPtr++;
                }
                if (lPtr->ident == NULL)
                    error("Invalid LP Type: %s.", CHAR(STRING_ELT(el, 0)));
                strncpy(opt->type, lPtr->ident, strlen(lPtr->ident));
                if (expExt == 0)
                    strncpy(ext, lPtr->ext, strlen(lPtr->ext));
                break;
            case AF_SPECTRUM:
                sPtr = spectType;
                while (sPtr->ident != NULL) {
                    if (strcmp(sPtr->ident, CHAR(STRING_ELT(el, 0))) == 0)
                        break;
                    sPtr++;
                }
                if (sPtr->ident == NULL)
                    error("Invalid SP Type: %s.", CHAR(STRING_ELT(el, 0)));
                strncpy(opt->type, sPtr->ident, strlen(sPtr->ident));
                if (setSPECTdefaults(opt) < 0) {
                    error("%s", getAsspMsg(asspMsgNum));
                }
                strncpy(opt->type, sPtr->ident, strlen(sPtr->ident));
                switch (sPtr->type) {
                case DT_FTPOW:
                case DT_FTAMP:
                case DT_FTSQR:
                    setDFTdefaults(opt);
                    break;
                case DT_FTLPS:
                    setLPSdefaults(opt);
                    break;
                case DT_FTCSS:
                    setCSSdefaults(opt);
                    break;
                case DT_FTCEP:
                    setCEPdefaults(opt);
                    break;
                default:
                    setAsspMsg(AEG_ERR_BUG,
                               "setSPECTdefaults: invalid default type");
                    error("%s.", getAsspMsg(asspMsgNum));
                    break;
                }
                if (expExt == 0)
                    strncpy(ext, sPtr->ext, strlen(sPtr->ext));
                break;
            default:
                break;
            }
            break;
        case WO_WINFUNC:
            wPtr = wfShortList;
            while (wPtr->code != NULL) {
                if (strcmp(wPtr->code, CHAR(STRING_ELT(el, 0))) == 0)
                    break;
                wPtr++;
            }
            if (wPtr->code == NULL)
                error("Invalid window function code %s.",
                      CHAR(STRING_ELT(el, 0)));
            strncpy(opt->winFunc, wPtr->code, strlen(wPtr->code));
            break;
            // /* These are not in libassp but in wrassp*/
        case WO_ENERGYNORM:
            if (INTEGER(el)[0])
                opt->options |= ACF_OPT_NORM;
            else
                opt->options &= ~ACF_OPT_NORM;
            break;
        case WO_LENGTHNORM:
            if (INTEGER(el)[0])
                opt->options |= ACF_OPT_MEAN;
            else
                opt->options &= ~ACF_OPT_MEAN;
            break;
        case WO_DIFF_OPT_BACKWARD:
            if (INTEGER(el)[0])
                opt->options |= DIFF_OPT_BACKWARD;
            else
                opt->options &= ~DIFF_OPT_BACKWARD;
            break;
        case WO_DIFF_OPT_CENTRAL:
            if (INTEGER(el)[0])
                opt->options |= DIFF_OPT_CENTRAL;
            else
                opt->options &= ~DIFF_OPT_CENTRAL;
            break;
        case WO_RMS_OPT_LINEAR:
            if (INTEGER(el)[0])
                opt->options |= RMS_OPT_LINEAR;
            else
                opt->options &= ~RMS_OPT_LINEAR;
            break;
        case WO_LPS_OPT_DEEMPH:
            if (INTEGER(el)[0])
                opt->options |= LPS_OPT_DEEMPH;
            else
                opt->options &= ~LPS_OPT_DEEMPH;
            break;
        case WO_OUTPUTEXT:
            if (TYPEOF(el) == NILSXP) {
                expExt = 0;
                break;
            }
            cPtr = strdup(CHAR(STRING_ELT(el, 0)));
            if (*cPtr != '.' && strlen(cPtr) != 0) {
                strncpy(ext, ".", strlen(".") + 1);
                strcat(ext, cPtr);
            } else {
                strncpy(ext, cPtr, strlen(cPtr) + 1);
            }
            free(cPtr);
            expExt = 1;
            switch (anaFunc->funcNum) {
            case AF_RFCANA:
                lPtr = lpType;
                while (lPtr->ident != NULL) {
                    if (strcmp(opt->type, lPtr->ident) == 0)
                        break;
                    lPtr++;
                }
                if (lPtr->ident == NULL)
                    error("Bad LP Type in memory (%s).", opt->type);
                if (strcmp(lPtr->ext, ext) == 0) {
                    expExt = 0;
                } else {
                    expExt = 1;
                }
                break;
            case AF_SPECTRUM:
                sPtr = spectType;
                while (sPtr->ident != NULL) {
                    if (strcmp(opt->type, sPtr->ident) == 0)
                        break;
                    sPtr++;
                }
                if (sPtr->ident == NULL)
                    error("Bad SP Type in memory (%s).", opt->type);
                if (strcmp(sPtr->ext, ext) == 0) {
                    expExt = 0;
                } else {
                    expExt = 1;

                }
                break;
            default:
                break;
            }
            break;
        case WO_TOFILE:
            toFile = INTEGER(el)[0] != 0;
            break;
        case WO_OUTPUTDIR:
            if (el == R_NilValue) {
                outDir = NULL;
                break;
            }
            outDir = strdup(CHAR(STRING_ELT(el, 0)));
            if (outDir[strlen(outDir) - 1] != DIR_SEP_CHR)
                outDir = strcat(outDir, DIR_SEP_STR);
            break;
        case WO_PBAR:
            pBar = el;
            break;
        default:
            break;
        }
    }

    /*
     * output extension might still be unset. For afdiff and for spectral 
     * analysis we need to take special care. In other cases it would be
     * weird to get here but we can safely use the default. 
     */
    if (strcmp(ext, "") == 0) {
        // could be explicitely set to ""
        if (!expExt) {
            switch (anaFunc->funcNum) {
            case AF_AFDIFF:
                // needs to be handled on a per file basis
                break;
            case AF_SPECTRUM:
                sPtr = spectType;
                while (sPtr->ident != NULL) {
                    if (strcmp(opt->type, sPtr->ident) == 0)
                        break;
                    sPtr++;
                }
                if (sPtr->ident == NULL)
                    error("Bad SP Type in memory (%s).", opt->type);
                strcpy(ext, sPtr->ext);
                break;
            default:
                strcpy(ext, anaFunc->defExt);
                break;
            }
        }
    }
    // do analysis

    /*
     * hook into the progressbar if present 
     */
    if (pBar != R_NilValue) {
        PROTECT(newVal = allocVector(INTSXP, 1));
        PROTECT(utilsPackage = eval(lang2(install("getNamespace"),
                                          ScalarString(mkChar("utils"))),
                                    R_GlobalEnv));
        INTEGER(newVal)[0] = 0;
        eval(lang4(install("setTxtProgressBar"), pBar, newVal, R_NilValue),
             utilsPackage);
    }

    /*
     * in memory only works for single input, not for multiple input so,
     * if toFile is false but there are multiple inputs set toFile to true 
     */
    toFile = toFile || length(inputs) != 1;

    /*
     * iterate over input files 
     */
    for (i = 0; i < length(inputs); i++) {
        name = strdup(CHAR(STRING_ELT(inputs, i)));
        inPtr = asspFOpen(strdup(name), AFO_READ, (DOBJ *) NULL);
        if (inPtr == NULL)
            error("%s (%s)", getAsspMsg(asspMsgNum), strdup(name));

        outPtr = (anaFunc->compProc) (inPtr, opt, (DOBJ *) NULL);
        if (outPtr == NULL) {
            asspFClose(inPtr, AFC_FREE);
            error("%s (%s)", getAsspMsg(asspMsgNum), strdup(name));
        }

        /*
         * input data object no longer needed 
         */
        asspFClose(inPtr, AFC_FREE);


        if (toFile) {
            /*
             * in toFile mode, all DOBJs are written to file we will later 
             * return the number of successful analyses 
             */

            /*
             * parse the input path to get directory (dPath), base file
             * name (bPath) and original extension (oExt) 
             */
            parsepath((char *) name, &dPath, &bPath, &oExt);
            /*
             * outName is the same except for extension unless outDir is
             * set 
             */
            strcpy(outName, "");
            if (outDir == NULL)
                strcat(outName, dPath);
            else
                strcat(outName, outDir);
            strcat(outName, bPath);
            /*
             * Extension may have to be set for afdiff but only if
             * extension is not set explicitely 
             */
            if (strcmp(ext, "") == 0 && !expExt) {
                switch (anaFunc->funcNum) {
                case AF_AFDIFF:
                    strcpy(ext, ".d");
                    oExt++;     /* skip period */
                    strcat(ext, oExt);
                    break;
                default:
                    error("Extension handling failed (performAssp).");
                    break;
                }
            }
            strcat(outName, ext);
            outPtr = asspFOpen(outName, AFO_WRITE, outPtr);
            if (outPtr == NULL) {
                asspFClose(outPtr, AFC_FREE);
                error("%s (%s)", getAsspMsg(asspMsgNum), strdup(outName));
            }
            if (asspFFlush(outPtr, 0) == -1) {
                asspFClose(outPtr, AFC_FREE);
                error("%s (%s)", getAsspMsg(asspMsgNum), strdup(outName));
            }
            asspFClose(outPtr, AFC_FREE);
        } else {
            res = dobj2AsspDataObj(outPtr);
            asspFClose(outPtr, AFC_KEEP);
        }

        free((char *) name);

        // if a progress bar was passed over, increment its value
        if (pBar != R_NilValue) {
            INTEGER(newVal)[0] = i + 1;
            eval(lang4
                 (install("setTxtProgressBar"), pBar, newVal, R_NilValue),
                 utilsPackage);
        }
    }
    free((void *) outDir);
    if (toFile) {
        // in toFile mode, the number of successful analyses is returned
        PROTECT(res = allocVector(INTSXP, 1));
        INTEGER(res)[0] = i;
    }
    // for the progress bar, to SEXPs were protected
    if (pBar != R_NilValue)
        UNPROTECT(2);

    // in toFile mode, the return value was protected
    if (toFile)
        UNPROTECT(1);
    return res;
}


/*
 * Wrapper functions for filtering and for ksv f0 analysis 
 */
DOBJ           *
computeFilter(DOBJ * inpDOp, AOPTS * anaOpts, DOBJ * outDOp)
{
    int             FILE_IN,
                    FILE_OUT,
                    CREATED;
    DOBJ           *filtDOp = NULL;
    if (inpDOp == NULL || anaOpts == NULL || outDOp != NULL) {
        setAsspMsg(AEB_BAD_ARGS, "computeFilter");
        return (NULL);
    }
    FILE_IN = FILE_OUT = CREATED = FALSE;
    /*
     * check input object 
     */
    if (inpDOp->fp != NULL) {
        if (inpDOp->numRecords <= 0) {
            setAsspMsg(AEF_EMPTY, inpDOp->filePath);
            return (NULL);
        }
        FILE_IN = TRUE;
    } else if (anaOpts == NULL ||
               (anaOpts != NULL && !(anaOpts->options & AOPT_INIT_ONLY))) {
        if (inpDOp->dataBuffer == NULL || inpDOp->bufNumRecs <= 0) {
            setAsspMsg(AED_NO_DATA, "(computeFilter)");
            return (NULL);
        }
    }


    filtDOp = createFilter(inpDOp, anaOpts);
    if (filtDOp == NULL) {
        return (NULL);
    }

    if ((outDOp = filterSignal(inpDOp, filtDOp, outDOp)) == NULL) {
        return (NULL);
    }
    filtDOp = destroyFilter(filtDOp);
    return (outDOp);
}

DOBJ           *
computeF0(DOBJ * inpDOp, AOPTS * anaOpts, DOBJ * outDOp)
{
    return computeKSV(inpDOp, anaOpts, outDOp, (DOBJ *) NULL);
}
