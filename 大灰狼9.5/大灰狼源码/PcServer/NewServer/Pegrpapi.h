///////////////////////////////////////////////////////////////////
// PEGRP32 API FUNCTIONS AND DEFINES      //
////////////////////////////////////////////

//---------------------------------------------------------------------------
// PEGRPAPI.H 
//---------------------------------------------------------------------------
// Copyright (c) 1992-2003, Gigasoft, Inc.  All Rights Reserved.
//---------------------------------------------------------------------------
// Include file for the GigaSoft ProEssentials Custom Controls 
//---------------------------------------------------------------------------

#ifndef __PEGRPAPI_H__
#define __PEGRPAPI_H__

#define PESTA_CENTER                0
#define PESTA_LEFT                  1
#define PESTA_RIGHT                 2

#define PEDO_DRIVERDEFAULT          0
#define PEDO_LANDSCAPE              1   
#define PEDO_PORTRAIT               2

#define PEVS_COLOR                  0
#define PEVS_MONO                   1
#define PEVS_MONOWITHSYMBOLS        2

#define PEFS_LARGE                  0
#define PEFS_MEDIUM                 1
#define PEFS_SMALL                  2

#define PEVB_NONE                   0
#define PEVB_TOP                    1
#define PEVB_BOTTOM                 2
#define PEVB_TOPANDBOTTOM           3 

#define PEAC_AUTO                   0
#define PEAC_NORMAL                 1
#define PEAC_LOG                    2

#define PEMC_HIDE                   0 
#define PEMC_SHOW                   1
#define PEMC_GRAYED                 2 

#define PECM_SHOW                   0 
#define PECM_GRAYED                 1 
#define PECM_HIDE                   2

#define PECMS_UNCHECKED             0 
#define PECMS_CHECKED               1 

#define PECML_TOP                   0
#define PECML_ABOVE_SEPARATOR       1
#define PECML_BELOW_SEPARATOR       2
#define PECML_BOTTOM                3

#define PEGPM_LINE                  0
#define PEGPM_BAR                   1
#define PEGPM_STICK                 4
#define PEGPM_POINT                 2
#define PEGPM_AREA                  3
#define PEGPM_AREASTACKED           4
#define PEGPM_AREASTACKEDPERCENT    5
#define PEGPM_BARSTACKED            6
#define PEGPM_BARSTACKEDPERCENT     7
#define PEGPM_POINTSPLUSBFL         8
#define PEGPM_POINTSPLUSBFLGRAPHED  9
#define PEGPM_HISTOGRAM             10
#define PEGPM_SPECIFICPLOTMODE      11
#define PEGPM_BUBBLE                12
#define PEGPM_POINTSPLUSBFC         13
#define PEGPM_POINTSPLUSBFCGRAPHED  14
#define PEGPM_POINTSPLUSSPLINE      15
#define PEGPM_SPLINE                16
#define PEGPM_POINTSPLUSLINE        17
#define PEGPM_HORIZONTALBAR         18
#define PEGPM_HORZBARSTACKED        19
#define PEGPM_HORZBARSTACKEDPERCENT 20
#define PEGPM_STEP                  21
#define PEGPM_RIBBON                22
#define PEGPM_CONTOURLINES          23
#define PEGPM_CONTOURCOLORS         24
#define PEGPM_HIGHLOWBAR            25
#define PEGPM_HIGHLOWLINE           26
#define PEGPM_HIGHLOWCLOSE          27
#define PEGPM_OPENHIGHLOWCLOSE      28
#define PEGPM_BOXPLOT               29

#define PECPS_NONE                  0
#define PECPS_XVALUE                1
#define PECPS_YVALUE                2
#define PECPS_XYVALUES              3

#define PEAUI_NONE                  0
#define PEAUI_ALL                   1
#define PEAUI_DISABLEKEYBOARD       2
#define PEAUI_DISABLEMOUSE          3

#define PEGLC_BOTH                  0
#define PEGLC_YAXIS                 1
#define PEGLC_XAXIS                 2
#define PEGLC_NONE                  3

#define PEAS_SUMPP                  51
#define PEAS_MINAP                  1
#define PEAS_MINPP                  52
#define PEAS_MAXAP                  2
#define PEAS_MAXPP                  53
#define PEAS_AVGAP                  3
#define PEAS_AVGPP                  54
#define PEAS_P1SDAP                 4
#define PEAS_P1SDPP                 55
#define PEAS_P2SDAP                 5
#define PEAS_P2SDPP                 56
#define PEAS_P3SDAP                 6
#define PEAS_P3SDPP                 57
#define PEAS_M1SDAP                 7
#define PEAS_M1SDPP                 58
#define PEAS_M2SDAP                 8
#define PEAS_M2SDPP                 59
#define PEAS_M3SDAP                 9
#define PEAS_M3SDPP                 60
#define PEAS_PARETO_ASC             90
#define PEAS_PARETO_DEC             91

#define PEPTGI_FIRSTPOINTS          0
#define PEPTGI_LASTPOINTS           1   

#define PEPTGV_SEQUENTIAL           0
#define PEPTGV_RANDOM               1

#define PEGPT_GRAPH                 0
#define PEGPT_TABLE                 1
#define PEGPT_BOTH                  2

#define PETW_GRAPHED                0
#define PETW_ALLSUBSETS             1

#define PEDLT_PERCENTAGE            0
#define PEDLT_VALUE                 1

#define PEMSC_NONE                  0
#define PEMSC_MIN                   1
#define PEMSC_MAX                   2
#define PEMSC_MINMAX                3

#define IDEXPORTBUTTON              1015
#define IDMAXIMIZEBUTTON            1016
#define IDORIGINALBUTTON            1109    

#define PEHS_NONE                   0
#define PEHS_SUBSET                 1
#define PEHS_POINT                  2
#define PEHS_GRAPH                  3
#define PEHS_TABLE                  4
#define PEHS_DATAPOINT              5
#define PEHS_ANNOTATION             6
#define PEHS_XAXISANNOTATION        7
#define PEHS_YAXISANNOTATION        8
#define PEHS_HORZLINEANNOTATION     9
#define PEHS_VERTLINEANNOTATION     10
#define PEHS_MAINTITLE              11
#define PEHS_SUBTITLE               12
#define PEHS_MULTISUBTITLE          13
#define PEHS_MULTIBOTTOMTITLE       14
#define PEHS_YAXISLABEL             15
#define PEHS_XAXISLABEL             16
#define PEHS_YAXIS                  17
#define PEHS_XAXIS                  18
#define PEHS_YAXISGRIDNUMBER        19
#define PEHS_RYAXISGRIDNUMBER       20
#define PEHS_XAXISGRIDNUMBER        21
#define PEHS_TXAXISGRIDNUMBER       22
#define PEHS_TABLEANNOTATION        23
#define PEHS_TABLEANNOTATION19      42
#define PEHS_ZAXISGRIDNUMBER        43

#define PESPM_NONE                  0
#define PESPM_HIGHLOWBAR            1
#define PESPM_HIGHLOWLINE           2
#define PESPM_HIGHLOWCLOSE          3
#define PESPM_OPENHIGHLOWCLOSE      4
#define PESPM_BOXPLOT               5

#define PEZIO_NORMAL                0
#define PEZIO_RECT                  1
#define PEZIO_LINE                  2

#define PETS_GRIDSTYLE              0
#define PETS_THICK                  1
#define PETS_DOT                    2
#define PETS_DASH                   3
#define PETS_1UNIT                  4
#define PETS_THIN                   5

#define PEZS_FRAMED_RECT            0
#define PEZS_RO2_NOT                1

#define PECPL_TOP_LEFT              0
#define PECPL_TOP_RIGHT             1

#define PELS_2_LINE                 0
#define PELS_1_LINE                 1
#define PELS_1_LINE_INSIDE_AXIS     2
#define PELS_1_LINE_TOP_OF_AXIS     3
#define PELS_1_LINE_INSIDE_OVERLAP  4

#define PEMAS_GROUP_ALL_AXES        0
#define PEMAS_SEPARATE_AXES         1

#define PETAHO_HORZ                 0
#define PETAHO_90                   1
#define PETAHO_270                  2

#define PETAL_TOP_CENTER            0
#define PETAL_TOP_LEFT              1
#define PETAL_LEFT_CENTER           2													
#define PETAL_BOTTOM_LEFT           3
#define PETAL_BOTTOM_CENTER         4
#define PETAL_BOTTOM_RIGHT          5
#define PETAL_RIGHT_CENTER          6
#define PETAL_TOP_RIGHT             7
#define PETAL_INSIDE_TOP_CENTER     8
#define PETAL_INSIDE_TOP_LEFT       9
#define PETAL_INSIDE_LEFT_CENTER    10
#define PETAL_INSIDE_BOTTOM_LEFT    11
#define PETAL_INSIDE_BOTTOM_CENTER  12
#define PETAL_INSIDE_BOTTOM_RIGHT   13
#define PETAL_INSIDE_RIGHT_CENTER   14
#define PETAL_INSIDE_TOP_RIGHT      15
#define PETAL_INSIDE_AXIS           100
#define PETAL_INSIDE_AXIS_0         100
#define PETAL_INSIDE_AXIS_1         101
#define PETAL_INSIDE_AXIS_2         102
#define PETAL_INSIDE_AXIS_3         103
#define PETAL_INSIDE_AXIS_4         104
#define PETAL_INSIDE_AXIS_5         105
#define PETAL_OUTSIDE_AXIS          200 
#define PETAL_OUTSIDE_AXIS_0        200
#define PETAL_OUTSIDE_AXIS_1        201
#define PETAL_OUTSIDE_AXIS_2        202
#define PETAL_OUTSIDE_AXIS_3        203
#define PETAL_OUTSIDE_AXIS_4        204
#define PETAL_OUTSIDE_AXIS_5        205
#define PETAL_INSIDE_TABLE          300
#define PETAL_OVERLAP_AXIS			400
#define PETAL_OVERLAP_AXIS_0		400
#define PETAL_OVERLAP_AXIS_1		401
#define PETAL_OVERLAP_AXIS_2		402
#define PETAL_OVERLAP_AXIS_3		403
#define PETAL_OVERLAP_AXIS_4		404
#define PETAL_OVERLAP_AXIS_5		405

#define PECONTROL_GRAPH             300
#define PECONTROL_PIE               302
#define PECONTROL_SGRAPH            304
#define PECONTROL_PGRAPH            308
#define PECONTROL_3D                312

#define PETAB_DROP_SHADOW           0
#define PETAB_SINGLE_LINE           1
#define PETAB_NO_BORDER             2
#define PETAB_INSET                 3

#define PETAAL_TOP_FULL_WIDTH       0			
#define PETAAL_TOP_LEFT             1
#define PETAAL_TOP_CENTER           2
#define PETAAL_TOP_RIGHT            3
#define PETAAL_BOTTOM_FULL_WIDTH    4
#define PETAAL_BOTTOM_LEFT          5
#define PETAAL_BOTTOM_CENTER        6
#define PETAAL_BOTTOM_RIGHT         7
#define PETAAL_TOP_TABLE_SPACED     8
#define PETAAL_BOTTOM_TABLE_SPACED  9
#define PETAAL_NEW_ROW              100

#define PETAJ_LEFT                  0 
#define PETAJ_CENTER                1 
#define PETAJ_RIGHT                 2 

#define PESTM_TICKS_INSIDE          0
#define PESTM_TICKS_OUTSIDE         1
#define PESTM_TICKS_HIDE            2

#define PESPL_PERCENTPLUSLABEL      0
#define PESPL_PERCENT               1
#define PESPL_LABEL                 2

#define PESH_MONOCHROME             0
#define PESH_BOTH                   1

#define PEAXD_INCLUDE_SAT_SUN       0
#define PEAXD_NO_WEEKENDS           1

#define PEPSC_NONE                  0
#define PEPSC_CURRENT_STYLE         1
#define PEPSC_DEFAULT_MONO          2

#define FIRST_DEFAULT_TAB           0
#define WIRE_FRAME_COLOR            32000
#define SOLID_SURFACE_COLOR         32001

#define PEDP_ENABLED                0
#define PEDP_DISABLED               1
#define PEDP_INSIDE_TOP             2

#define PETLT_12HR_AM_PM            0
#define PETLT_12HR_NO_AM_PM         1 
#define PETLT_24HR                  2 

#define PEDLT_3_CHAR                0 
#define PEDLT_1_CHAR                1 
#define PEDLT_NO_DAY_PROMPT         2
#define PEDLT_NO_DAY_NUMBER         3

#define PEMLT_3_CHAR                0 
#define PEMLT_1_CHAR                1 
#define PEMLT_NO_MONTH_PROMPT       2

// HORIZONTAL LINE ANNOTATIONS CAN BE WITH RESPECT TO RIGHT Y AXIS COORDINATES
// BY ADDING 1000 TO THE FOLLOWING CONSTANTS 
#define PELT_THINSOLID              0
#define PELT_DASH                   1
#define PELT_DOT                    2
#define PELT_DASHDOT                3
#define PELT_DASHDOTDOT             4
#define PELT_MEDIUMSOLID            5
#define PELT_THICKSOLID             6
#define PELAT_GRIDTICK              7
#define PELAT_GRIDLINE              8
#define PELT_MEDIUMTHINSOLID        9
#define PELT_MEDIUMTHICKSOLID       10
#define PELT_EXTRATHICKSOLID        11
#define PELT_EXTRATHINSOLID         12
#define PELT_EXTRAEXTRATHINSOLID    13
#define PELAT_GRIDTICKII            14
#define PELAT_GRIDLINEII            15
									  
#define PEPS_SMALL                  0
#define PEPS_MEDIUM                 1
#define PEPS_LARGE                  2
#define PEPS_MICRO                  3

#define PEPT_PLUS                   0
#define PEPT_CROSS                  1
#define PEPT_DOT                    2
#define PEPT_DOTSOLID               3
#define PEPT_SQUARE                 4
#define PEPT_SQUARESOLID            5
#define PEPT_DIAMOND                6
#define PEPT_DIAMONDSOLID           7
#define PEPT_UPTRIANGLE             8
#define PEPT_UPTRIANGLESOLID        9
#define PEPT_DOWNTRIANGLE           10
#define PEPT_DOWNTRIANGLESOLID      11
#define PEPT_DASH                   72
#define PEPT_PIXEL                  73
#define PEPT_ARROW_N                92
#define PEPT_ARROW_NE               93
#define PEPT_ARROW_E                94
#define PEPT_ARROW_SE               95
#define PEPT_ARROW_S                96
#define PEPT_ARROW_SW               97
#define PEPT_ARROW_W                98
#define PEPT_ARROW_NW               99

#define PEADL_NONE                  0
#define PEADL_DATAVALUES            1
#define PEADL_POINTLABELS           2
#define PEADL_DATAPOINTLABELS       3

#define PEAZ_NONE                   0
#define PEAZ_HORIZONTAL             1
#define PEAZ_VERTICAL               2
#define PEAZ_HORZANDVERT            3

#define PEBFD_2ND                   0
#define PEBFD_3RD                   1
#define PEBFD_4TH                   2

#define PEBS_SMALL                  0
#define PEBS_MEDIUM                 1
#define PEBS_LARGE                  2

#define PECG_COARSE                 0
#define PECG_MEDIUM                 1
#define PECG_FINE                   2

#define PEAE_NONE                   0
#define PEAE_ALLSUBSETS             1
#define PEAE_INDSUBSETS             2

#define PECM_NOCURSOR               0
#define PECM_POINT                  1
#define PECM_DATACROSS              2
#define PECM_DATASQUARE             3
#define PECM_FLOATINGY              4
#define PECM_FLOATINGXY             5
#define PECM_FLOATINGXONLY          6
#define PECM_FLOATINGYONLY          7

#define PEHS_HORIZONTAL             0       /* ----- */
#define PEHS_VERTICAL               1       /* ||||| */
#define PEHS_FDIAGONAL              2       /* \\\\\ */
#define PEHS_BDIAGONAL              3       /* ///// */
#define PEHS_CROSS                  4       /* +++++ */
#define PEHS_DIAGCROSS              5       /* xxxxx */

#define PEGS_NO_GRADIENT            0
#define PEGS_VERTICAL               1
#define PEGS_HORIZONTAL             2

#define PEBS_NO_BMP                 0
#define PEBS_STRETCHBLT             1
#define PEBS_TILED_BITBLT           2
#define PEBS_BITBLT_TOP_LEFT        3
#define PEBS_BITBLT_TOP_CENTER      4
#define PEBS_BITBLT_TOP_RIGHT       5
#define PEBS_BITBLT_BOTTOM_LEFT     6
#define PEBS_BITBLT_BOTTOM_CENTER   7
#define PEBS_BITBLT_BOTTOM_RIGHT    8
#define PEBS_BITBLT_CENTER          9

#define PEQS_NO_STYLE               0
#define PEQS_LIGHT_INSET            1
#define PEQS_LIGHT_SHADOW           2
#define PEQS_LIGHT_LINE             3
#define PEQS_LIGHT_NO_BORDER        4
#define PEQS_MEDIUM_INSET           5
#define PEQS_MEDIUM_SHADOW          6
#define PEQS_MEDIUM_LINE            7
#define PEQS_MEDIUM_NO_BORDER       8
#define PEQS_DARK_INSET             9
#define PEQS_DARK_SHADOW            10
#define PEQS_DARK_LINE              11
#define PEQS_DARK_NO_BORDER         12
 
// GRAPH ANNOTATIONS CAN BE WITH RESPECT TO RIGHT Y AXIS COORDINATES
// BY ADDING 1000 TO THE FOLLOWING CONSTANTS
#define PEGAT_NOSYMBOL               0
#define PEGAT_PLUS                   1
#define PEGAT_CROSS                  2
#define PEGAT_DOT                    3
#define PEGAT_DOTSOLID               4
#define PEGAT_SQUARE                 5
#define PEGAT_SQUARESOLID            6
#define PEGAT_DIAMOND                7
#define PEGAT_DIAMONDSOLID           8
#define PEGAT_UPTRIANGLE             9
#define PEGAT_UPTRIANGLESOLID        10
#define PEGAT_DOWNTRIANGLE           11
#define PEGAT_DOWNTRIANGLESOLID      12
#define PEGAT_SMALLPLUS              13
#define PEGAT_SMALLCROSS             14
#define PEGAT_SMALLDOT               15
#define PEGAT_SMALLDOTSOLID          16
#define PEGAT_SMALLSQUARE            17
#define PEGAT_SMALLSQUARESOLID       18
#define PEGAT_SMALLDIAMOND           19
#define PEGAT_SMALLDIAMONDSOLID      20
#define PEGAT_SMALLUPTRIANGLE        21
#define PEGAT_SMALLUPTRIANGLESOLID   22
#define PEGAT_SMALLDOWNTRIANGLE      23
#define PEGAT_SMALLDOWNTRIANGLESOLID 24
#define PEGAT_LARGEPLUS              25
#define PEGAT_LARGECROSS             26
#define PEGAT_LARGEDOT               27
#define PEGAT_LARGEDOTSOLID          28
#define PEGAT_LARGESQUARE            29
#define PEGAT_LARGESQUARESOLID       30
#define PEGAT_LARGEDIAMOND           31
#define PEGAT_LARGEDIAMONDSOLID      32
#define PEGAT_LARGEUPTRIANGLE        33
#define PEGAT_LARGEUPTRIANGLESOLID   34
#define PEGAT_LARGEDOWNTRIANGLE      35
#define PEGAT_LARGEDOWNTRIANGLESOLID 36

#define PEGAT_POINTER                37 

#define PEGAT_THINSOLIDLINE          38
#define PEGAT_DASHLINE               39
#define PEGAT_DOTLINE                40
#define PEGAT_DASHDOTLINE            41
#define PEGAT_DASHDOTDOTLINE         42
#define PEGAT_MEDIUMSOLIDLINE        43
#define PEGAT_THICKSOLIDLINE         44
#define PEGAT_LINECONTINUE           45

#define PEGAT_TOPLEFT                46
#define PEGAT_BOTTOMRIGHT            47

#define PEGAT_RECT_THIN              48
#define PEGAT_RECT_DASH              49
#define PEGAT_RECT_DOT               50
#define PEGAT_RECT_DASHDOT           51
#define PEGAT_RECT_DASHDOTDOT        52
#define PEGAT_RECT_MEDIUM            53
#define PEGAT_RECT_THICK             54
#define PEGAT_RECT_FILL              55

#define PEGAT_ROUNDRECT_THIN         56
#define PEGAT_ROUNDRECT_DASH         57
#define PEGAT_ROUNDRECT_DOT          58
#define PEGAT_ROUNDRECT_DASHDOT      59
#define PEGAT_ROUNDRECT_DASHDOTDOT   60
#define PEGAT_ROUNDRECT_MEDIUM       61
#define PEGAT_ROUNDRECT_THICK        62
#define PEGAT_ROUNDRECT_FILL         63

#define PEGAT_ELLIPSE_THIN           64
#define PEGAT_ELLIPSE_DASH           65
#define PEGAT_ELLIPSE_DOT            66
#define PEGAT_ELLIPSE_DASHDOT        67
#define PEGAT_ELLIPSE_DASHDOTDOT     68
#define PEGAT_ELLIPSE_MEDIUM         69  
#define PEGAT_ELLIPSE_THICK          70
#define PEGAT_ELLIPSE_FILL           71

#define PEGAT_DASH                   72
#define PEGAT_PIXEL                  73

#define PEGAT_STARTPOLY              74
#define PEGAT_ADDPOLYPOINT           75
#define PEGAT_ENDPOLYGON             76
#define PEGAT_ENDPOLYLINE_THIN       77
#define PEGAT_ENDPOLYLINE_MEDIUM     78
#define PEGAT_ENDPOLYLINE_THICK      79
#define PEGAT_ENDPOLYLINE_DASH	     80
#define PEGAT_ENDPOLYLINE_DOT	     81
#define PEGAT_ENDPOLYLINE_DASHDOT    82
#define PEGAT_ENDPOLYLINE_DASHDOTDOT 83

#define PEGAT_STARTTEXT              84
#define PEGAT_ADDTEXT                85
#define PEGAT_PARAGRAPH              86

#define PEGAT_MEDIUMTHINSOLID        87
#define PEGAT_MEDIUMTHICKSOLID       88
#define PEGAT_EXTRATHICKSOLID        89
#define PEGAT_EXTRATHINSOLID         90
#define PEGAT_EXTRAEXTRATHINSOLID    91

#define PEGAT_ARROW_N                92
#define PEGAT_ARROW_NE               93
#define PEGAT_ARROW_E                94
#define PEGAT_ARROW_SE               95
#define PEGAT_ARROW_S                96
#define PEGAT_ARROW_SW               97
#define PEGAT_ARROW_W                98
#define PEGAT_ARROW_NW               99

#define PEGAT_SMALL_OBSTACLE         100
#define PEGAT_MEDIUM_OBSTACLE        101
#define PEGAT_LARGE_OBSTACLE         102
#define PEGAT_RECT_OBSTACLE          103 
#define PEGAT_NOSYMBOL_MOVABLE       104

#define PEDTM_NONE                   0
#define PEDTM_VB                     1
#define PEDTM_DELPHI                 2

#define PESC_POLAR                   0
#define PESC_SMITH                   1
#define PESC_ROSE                    2
#define PESC_ADMITTANCE              3

#define PESA_ALL                     0
#define PESA_AXISLABELS              1
#define PESA_GRIDNUMBERS             2
#define PESA_NONE                    3
#define PESA_LABELONLY               4
#define PESA_EMPTY                   5

#define PEMPS_NONE                   0
#define PEMPS_SMALL                  1
#define PEMPS_MEDIUM                 2
#define PEMPS_LARGE                  3

#define PESS_NONE                    0
#define PESS_FINANCIAL               1

#define PELL_TOP                     0
#define PELL_BOTTOM                  1
#define PELL_LEFT                    2
#define PELL_RIGHT                   3

#define PEHSS_SMALL                  0
#define PEHSS_MEDIUM                 1
#define PEHSS_LARGE                  2

#define PEHSS_SCROLLING_POINTLABELS  0
#define PEHSS_STATIONARY_POINTLABELS 1

#define PEETD_METAFILE               0
#define PEETD_BMP                    1
#define PEETD_JPEG                   2
#define PEETD_PNG                    3
#define PEETD_TEXT                   4

#define PEEDD_CLIPBOARD              0
#define PEEDD_FILE                   1
#define PEEDD_PRINTER                2

#define PEESD_NO_SIZE_OR_PIXEL       0
#define PEESD_MILLIMETERS            1
#define PEESD_INCHES                 2
#define PEESD_POINTS                 3

#define PEGAM_NOT_MOVEABLE           0
#define PEGAM_POINTER                1

#define PEDS_NONE                    0
#define PEDS_SHADOWS                 1
#define PEDS_3D	                     2

#define PEGS_THIN                    0
#define PEGS_THICK                   1
#define PEGS_DOT                     2
#define PEGS_DASH                    3
#define PEGS_ONEPIXEL                4

#define PEFVP_AUTO                   0
#define PEFVP_VERT                   1
#define PEFVP_HORZ                   2
#define PEFVP_SLANTED                3

#define PEMAS_NONE                   0
#define PEMAS_THIN                   1
#define PEMAS_MEDIUM                 2
#define PEMAS_THICK                  3
#define PEMAS_THICKPLUSTICK          4

#define PERI_INCBY15                 0 
#define PERI_INCBY10                 1 
#define PERI_INCBY5                  2 
#define PERI_INCBY2                  3 
#define PERI_INCBY1                  4 
#define PERI_DECBY1                  5 
#define PERI_DECBY2                  6 
#define PERI_DECBY5                  7 
#define PERI_DECBY10                 8 
#define PERI_DECBY15                 9 

#define PERD_WIREFRAME               0 
#define PERD_PLOTTINGMETHOD          1 
#define PERD_FULLDETAIL              2 

#define PESBB_WHILEROTATING          0 
#define PESBB_ALWAYS                 1 
#define PESBB_NEVER                  2 

// PolyModes
#define PEPM_SURFACEPOLYGONS         1 
#define PEPM_3DBAR                   2 
#define PEPM_POLYGONDATA             3 
#define PEPM_SCATTER                 4 

// Plotting Methods
#define PEPLM_WIREFRAME              0
#define PEPLM_SURFACE                1
#define PEPLM_SURFACE_W_SHADING      2
#define PEPLM_SURFACE_W_PIXELS       3
#define PEPLM_SURFACE_W_CONTOUR      4

// Plotting Methods for Scatter Graph
#define PEPLM_POINTS                 0
#define PEPLM_LINES                  1
#define PEPLM_POINTS_AND_LINES       2

#define PESC_NONE                    0 
#define PESC_TOPLINES                1 
#define PESC_BOTTOMLINES             2 
#define PESC_TOPCOLORS               3 
#define PESC_BOTTOMCOLORS            4 

#define PESS_WHITESHADING            0 
#define PESS_COLORSHADING            1 

#define PETS_NO_TEXT                 0
#define PETS_BOLD_TEXT               1
#define PETS_ALL_TEXT                2

#define PEMWF_VERT_SCROLL            0
#define PEMWF_HORZ_SCROLL            1
#define PEMWF_NO_SCROLL              2

#define PESB_MOUSE_WHEEL_UP          100
#define PESB_MOUSE_WHEEL_DOWN        101

typedef struct tagGLOBALPROPERTIES   // defined but we recommend using PEload/PEloadpartial instead 
{
    // GLOBAL SCOPE //
    UINT    nObjectType;            // read only
    char    szMainTitle[48];
    char    szSubTitle[48];
    UINT    nSubsets;
    UINT    nPoints;
    
    BOOL    bMonoWithSymbols;
    UINT    nDefOrientation;
    BOOL    bPrepareImages;
    BOOL    b3dDialogs;
    BOOL    bDataShadows;
    
    BOOL    bAllowCustomization;
    BOOL    bAllowExporting;
    BOOL    bAllowMaximization;
    BOOL    bAllowPopup;
    BOOL    bAllowUserInterface;
    
    UINT    nPageWidth;
    UINT    nPageHeight;
    RECT    rectLogicalLoc;
    
    BOOL    bCustom;
    BOOL    nViewingStyle;
    BOOL    nCViewingStyle;
    
    DWORD   dwMonoDeskColor;
    DWORD   dwMonoTextColor;
    DWORD   dwMonoShadowColor;   
    DWORD   dwMonoGraphForeColor;
    DWORD   dwMonoGraphBackColor;
    DWORD   dwMonoTableForeColor;
    DWORD   dwMonoTableBackColor;
    
    DWORD   dwCMonoDeskColor;
    DWORD   dwCMonoTextColor;
    DWORD   dwCMonoShadowColor;
    DWORD   dwCMonoGraphForeColor;
    DWORD   dwCMonoGraphBackColor;
    DWORD   dwCMonoTableForeColor;
    DWORD   dwCMonoTableBackColor;
    
    DWORD   dwDeskColor;
    DWORD   dwTextColor;
    DWORD   dwShadowColor;        
    DWORD   dwGraphForeColor;
    DWORD   dwGraphBackColor;
    DWORD   dwTableForeColor;
    DWORD   dwTableBackColor;
    
    DWORD   dwCDeskColor;
    DWORD   dwCTextColor;
    DWORD   dwCShadowColor;
    DWORD   dwCGraphForeColor;
    DWORD   dwCGraphBackColor;
    DWORD   dwCTableForeColor;
    DWORD   dwCTableBackColor;
    
    UINT    nDataPrecision;
    UINT    nCDataPrecision;
    
    UINT    nFontSize;
    UINT    nCFontSize;
    
    char    szMainTitleFont[48];
    BOOL    bMainTitleBold;
    BOOL    bMainTitleItalic;
    BOOL    bMainTitleUnderline;
    char    szCMainTitleFont[48];
    BOOL    bCMainTitleBold;
    BOOL    bCMainTitleItalic;
    BOOL    bCMainTitleUnderline;
    char    szSubTitleFont[48];
    BOOL    bSubTitleBold;
    BOOL    bSubTitleItalic;
    BOOL    bSubTitleUnderline;    
    char    szCSubTitleFont[48];
    BOOL    bCSubTitleBold;
    BOOL    bCSubTitleItalic;
    BOOL    bCSubTitleUnderline;    
    char    szLabelFont[48];
    BOOL    bLabelBold;
    BOOL    bLabelItalic;
    BOOL    bLabelUnderline;    
    char    szCLabelFont[48];
    BOOL    bCLabelBold;
    BOOL    bCLabelItalic;
    BOOL    bCLabelUnderline;   
    char    szTableFont[48];    
    char    szCTableFont[48];
    
    // Version 2.0 //
    BOOL    bAllowSubsetHotSpots;
    BOOL    bAllowPointHotSpots;
    
} GLOBALPROPERTIES; 

typedef struct tagGRAPHPROPERTIES  // defined but we recommend using PEload/PEloadpartial instead 
{
    // GRAPH SCOPE //
    char    szXAxisLabel[48];
    char    szYAxisLabel[48];
    UINT    nVBoundTypes;
    double  fUpperBoundValue;
    double  fLowerBoundValue;
    char    szUpperBoundText[48];
    char    szLowerBoundText[48];
    UINT    nYAxisScaleControl;
    UINT    nManualScaleControlY;
    double  fManualMinY;
    double  fManualMaxY;
    BOOL    bNoScrollingSubsetControl;  
    UINT    nScrollingSubsets;
    UINT    nCScrollingSubsets;
    UINT    nPlottingMethod;
    UINT    nCPlottingMethod;
    UINT    nGridLineControl;
    UINT    nCGridLineControl;
    BOOL    bGridInFront;
    BOOL    bCGridInFront;
    BOOL    bTreatCompsAsNormal;
    BOOL    bCTreatCompsAsNormal;
    UINT    nComparisonSubsets;

    BOOL    bNoStackedData;
    UINT    nPointsToGraphInit;
    UINT    nPointsToGraphVersion;
    UINT    nCPointsToGraphVersion;
    UINT    nPointsToGraph;
    UINT    nCPointsToGraph;
    BOOL    bForceVerticalPoints;
    BOOL    bCForceVerticalPoints;
    UINT    nGraphPlusTable;
    UINT    nCGraphPlusTable;
    UINT    nTableWhat;
    UINT    nCTableWhat;
    
    // Version 2.0 //
    BOOL    bAllowDataHotSpots;
    BOOL    bAllowGraphHotSpots;
    BOOL    bAllowTableHotSpots;
    BOOL    bAllowCoordPrompting;
    BOOL    bMarkDataPoints;
    BOOL    bCMarkDataPoints;
    
    UINT    nRYAxisComparisonSubsets;
    UINT    nRYAxisScaleControl;
    UINT    nRYAxisInitialScaleForY;
    UINT    nRYAxisManualScaleControlY; 
    double  fRYAxisManualMinY;
    double  fRYAxisManualMaxY;
    char    szRYAxisLabel[48];
    
    BOOL    bAllowPlotCustomization;
    BOOL    bAllowHistogram;
    int     nSpecificPlotMode;
    BOOL    bNegativeFromXAxis;
    
    BOOL    bManualYAxisTicknLine;
    double  fManualYAxisTick;
    double  fManualYAxisLine;
    BOOL    bManualRYAxisTicknLine;
    double  fManualRYAxisTick;
    double  fManualRYAxisLine;
    
    int     nTargetPointsToTable;
    int     nAltFreqThreshold;
    
    double   fNullDataValue;
    
    int     nPointSize;
    BOOL    bAllowBestFitCurve;
    int     nBestFitDegree;
    BOOL    bAllowSpline;
    int     nCurveGranularity;
    
    double  fManualStackedMaxY;
    
    int     nMaxPointsToGraph;
    BOOL    bDisableRandomPointsToGraph;
    
    BOOL    bAllowLine;
    BOOL    bAllowBar;
    BOOL    bAllowPoint;
    BOOL    bAllowBestFitLine;
    BOOL    bAllowArea;
    
    int     nAllowZooming;
    
    BOOL    bForceRightYAxis;
    BOOL    bAllowPointsPlusLine;
    BOOL    bAllowPointsPlusSpline;
    BOOL    bAllowBestFitLineII;
    BOOL    bAllowBestFitCurveII;
    int     nSymbolFrequency;
    BOOL    bAppendToEnd;
} GRAPHPROPERTIES;  

typedef struct tagSGRAPHPROPERTIES // defined but we recommend using PEload/PEloadpartial instead 
{
    // SCIENTIFIC GRAPH SCOPE //       
    char    szXAxisLabel[48];
    char    szYAxisLabel[48];
    UINT    nVBoundTypes;
    double  fUpperBoundValue;
    double  fLowerBoundValue;
    char    szUpperBoundText[48];
    char    szLowerBoundText[48];
    UINT    nYAxisScaleControl;
    UINT    nManualScaleControlY;
    double  fManualMinY;
    double  fManualMaxY;
    BOOL    bNoScrollingSubsetControl;  
    UINT    nScrollingSubsets;
    UINT    nCScrollingSubsets;
    UINT    nPlottingMethod;
    UINT    nCPlottingMethod;
    UINT    nGridLineControl;
    UINT    nCGridLineControl;
    BOOL    bGridInFront;
    BOOL    bCGridInFront;
    BOOL    bTreatCompsAsNormal;
    BOOL    bCTreatCompsAsNormal;
    UINT    nComparisonSubsets;

    UINT    nXAxisScaleControl;
    UINT    nManualScaleControlX;
    double  fManualMinX;
    double  fManualMaxX;
    BOOL    bGraphDataLabels;
    BOOL    bCGraphDataLabels;
    
    // Version 2.0 //
    BOOL    bAllowDataHotSpots;
    BOOL    bAllowGraphHotSpots;
    BOOL    bAllowCoordPrompting;
    BOOL    bMarkDataPoints;
    BOOL    bCMarkDataPoints;

    UINT    nRYAxisComparisonSubsets;
    UINT    nRYAxisScaleControl;
    UINT    nRYAxisInitialScaleForY;
    UINT    nRYAxisManualScaleControlY; 
    double  fRYAxisManualMinY;
    double  fRYAxisManualMaxY;
    char    szRYAxisLabel[48];

    BOOL    bAllowPlotCustomization;
    BOOL    bAllowBubble;
    int     nBubbleSize;
    BOOL    bNegativeFromXAxis;
    int     nAllowDataLabels;
    
    BOOL    bManualXAxisTicknLine;
    double  fManualXAxisTick;
    double  fManualXAxisLine;
    BOOL    bManualYAxisTicknLine;
    double  fManualYAxisTick;
    double  fManualYAxisLine;
    BOOL    bManualRYAxisTicknLine;
    double  fManualRYAxisTick;
    double  fManualRYAxisLine;
    
    double  fNullDataValue;
    
    int     nPointSize;
    BOOL    bAllowBestFitCurve;
    int     nBestFitDegree;
    BOOL    bAllowSpline;
    int     nCurveGranularity;
    
    BOOL    bAllowLine;
    BOOL    bAllowPoint;
    BOOL    bAllowBestFitLine;
    BOOL    bAllowStick;
    
    int     nAllowZooming;
    BOOL    bZoomMode;
    double  fZoomMinX;
    double  fZoomMaxX;
    double  fZoomMinY;
    double  fZoomMaxY;
    
    BOOL    bForceRightYAxis;
    BOOL    bAllowPointsPlusLine;
    BOOL    bAllowPointsPlusSpline;
    int     nSymbolFrequency;
} SGRAPHPROPERTIES;

typedef struct tagPIEPROPERTIES	 // defined but we recommend using PEload/PEloadpartial instead 
{
    // PIE SCOPE //
    UINT    nGroupingPercent;
    UINT    nCGroupingPercent;
    UINT    nDataLabelType;
    UINT    nCDataLabelType;
    UINT    nAutoExplode;
} PIEPROPERTIES;                

typedef struct tagSCROLLPARMS
{
    int     nVmin;  // vertical scrollbar minimum
    int     nVmax;  // vertical scrollbar maximum
    int     nVpos;  // vertical scrollbar position
    int     nHmin;  // horizontal scrollbar minimum
    int     nHmax;  // horizontal scrollbar maximum
    int     nHpos;  // horizontal scrollbar position
} SCROLLPARMS;

typedef struct tagHOTSPOTDATA
{
    RECT    rectHotSpot;
    UINT    nHotSpotType;
    UINT    w1;
    UINT    w2;
} HOTSPOTDATA; 

typedef struct tagKEYDOWNDATA
{
    UINT nChar;
    UINT nRepCnt;
    UINT nFlags;
} KEYDOWNDATA; 

typedef struct tagGRAPHLOC
{
    int     nAxis;
    double  fXval;
    double  fYval;
} GRAPHLOC; 

typedef struct tagTM
{
    int     nMonth;
    int     nDay;  
    int     nYear; 
    int     nHour; 
    int     nMinute;
    int     nSecond;
    int     nWeekDay;
    int     nYearDay;
} TM;

typedef struct tagEXTRAAXIS
{
    int      nSize;
    float    fMin;
    float    fMax;
    char     szLabel[64];
    float    fManualLine;
    float    fManualTick;
    char     szFormat[16];
    int      nShowAxis;
    int      nShowTickMark;
    int      bInvertedAxis;
    int      bLogScale;
    DWORD    dwColor;
} EXTRAAXIS; 

typedef struct tagCUSTOMGRIDNUMBERS
{
    int    nAxisType;    // 0=Y, 1=RIGHT Y, 2=X, 3=TOP X
    int    nAxisIndex;   // only used for y and ry axes, index number relates to PEP_nWORKINGAXIS
    double dNumber;      // number to format
    char   szData[48];   // With PEvget, default format string  ...  With PEvset, completed formatted string
} CUSTOMGRIDNUMBERS; 


#define PEP_nOBJECTTYPE             2100 
#define PEP_szMAINTITLE             2105
#define PEP_szSUBTITLE              2110
#define PEP_nSUBSETS                2115
#define PEP_nPOINTS                 2120
#define PEP_szaSUBSETLABELS         2125
#define PEP_szaPOINTLABELS          2130
#define PEP_faXDATA                 2135
#define PEP_faYDATA                 2140
#define PEP_bMONOWITHSYMBOLS        2145
#define PEP_nDEFORIENTATION         2150
#define PEP_bPREPAREIMAGES          2155
#define PEP_b3DDIALOGS              2160
#define PEP_bALLOWCUSTOMIZATION     2165
#define PEP_bALLOWEXPORTING         2170
#define PEP_bALLOWMAXIMIZATION      2175
#define PEP_bALLOWPOPUP             2180
#define PEP_nALLOWUSERINTERFACE     2185
#define PEP_bALLOWUSERINTERFACE     2185
#define PEP_dwaSUBSETCOLORS         2190
#define PEP_dwaSUBSETSHADES         2195
#define PEP_nPAGEWIDTH              2200
#define PEP_nPAGEHEIGHT             2205
#define PEP_rectLOGICALLOC          2210
#define PEP_bDIRTY                  2215
#define PEP_bDIALOGSHOWN            2220
#define PEP_bCUSTOM                 2225
#define PEP_nVIEWINGSTYLE           2230
#define PEP_nCVIEWINGSTYLE          2235
#define PEP_nDATASHADOWS            2240
#define PEP_nCDATASHADOWS           2245
#define PEP_bDATASHADOWS            2240
#define PEP_bCDATASHADOWS           2245
#define PEP_dwMONODESKCOLOR         2250
#define PEP_dwMONOTEXTCOLOR         2255
#define PEP_dwMONOSHADOWCOLOR       2260
#define PEP_dwMONOGRAPHFORECOLOR    2265
#define PEP_dwMONOGRAPHBACKCOLOR    2270
#define PEP_dwMONOTABLEFORECOLOR    2275
#define PEP_dwMONOTABLEBACKCOLOR    2280
#define PEP_dwCMONODESKCOLOR        2285
#define PEP_dwCMONOTEXTCOLOR        2290
#define PEP_dwCMONOSHADOWCOLOR      2295
#define PEP_dwCMONOGRAPHFORECOLOR   2300
#define PEP_dwCMONOGRAPHBACKCOLOR   2305
#define PEP_dwCMONOTABLEFORECOLOR   2310
#define PEP_dwCMONOTABLEBACKCOLOR   2315
#define PEP_dwDESKCOLOR             2320
#define PEP_dwTEXTCOLOR             2325
#define PEP_dwSHADOWCOLOR           2330
#define PEP_dwGRAPHFORECOLOR        2335
#define PEP_dwGRAPHBACKCOLOR        2340
#define PEP_dwTABLEFORECOLOR        2345
#define PEP_dwTABLEBACKCOLOR        2350
#define PEP_dwCDESKCOLOR            2355
#define PEP_dwCTEXTCOLOR            2360
#define PEP_dwCSHADOWCOLOR          2365
#define PEP_dwCGRAPHFORECOLOR       2370
#define PEP_dwCGRAPHBACKCOLOR       2375
#define PEP_dwCTABLEFORECOLOR       2380
#define PEP_dwCTABLEBACKCOLOR       2385
#define PEP_dwWDESKCOLOR            2390 
#define PEP_dwWTEXTCOLOR            2395
#define PEP_dwWSHADOWCOLOR          2400
#define PEP_dwWGRAPHFORECOLOR       2405
#define PEP_dwWGRAPHBACKCOLOR       2410
#define PEP_dwWTABLEFORECOLOR       2415
#define PEP_dwWTABLEBACKCOLOR       2420
#define PEP_nDATAPRECISION          2425
#define PEP_nCDATAPRECISION         2430
#define PEP_nMAXDATAPRECISION       2431
#define PEP_nFONTSIZE               2435
#define PEP_nCFONTSIZE              2440
#define PEP_szMAINTITLEFONT         2445
#define PEP_bMAINTITLEBOLD          2450
#define PEP_bMAINTITLEITALIC        2455
#define PEP_bMAINTITLEUNDERLINE     2460
#define PEP_szCMAINTITLEFONT        2465
#define PEP_bCMAINTITLEBOLD         2470
#define PEP_bCMAINTITLEITALIC       2475
#define PEP_bCMAINTITLEUNDERLINE    2480   
#define PEP_szSUBTITLEFONT          2485
#define PEP_bSUBTITLEBOLD           2490
#define PEP_bSUBTITLEITALIC         2495
#define PEP_bSUBTITLEUNDERLINE      2500
#define PEP_szCSUBTITLEFONT         2505
#define PEP_bCSUBTITLEBOLD          2510
#define PEP_bCSUBTITLEITALIC        2515
#define PEP_bCSUBTITLEUNDERLINE     2520
#define PEP_szLABELFONT             2525
#define PEP_bLABELBOLD              2530
#define PEP_bLABELITALIC            2535
#define PEP_bLABELUNDERLINE         2540
#define PEP_szCLABELFONT            2545
#define PEP_bCLABELBOLD             2550
#define PEP_bCLABELITALIC           2555
#define PEP_bCLABELUNDERLINE        2560
#define PEP_szTABLEFONT             2565
#define PEP_szCTABLEFONT            2570
#define PEP_bCACHEBMP               2574
#define PEP_hMEMBITMAP              2575
#define PEP_hMEMDC                  2580
#define PEP_bALLOWSUBSETHOTSPOTS    2600
#define PEP_bALLOWPOINTHOTSPOTS     2605
#define PEP_structHOTSPOTDATA       2610
#define PEP_structKEYDOWNDATA       2612
#define PEP_bAUTOIMAGERESET         2615
#define PEP_bALLOWTITLESDIALOG      2616
#define PEP_nCURSORMODE             2617
#define PEP_nCURSORSUBSET           2618
#define PEP_nCURSORPOINT            2619
#define PEP_nCURSORPROMPTSTYLE      2620
#define PEP_bCURSORPROMPTTRACKING   2621
#define PEP_bMOUSECURSORCONTROL     2622
#define PEP_bALLOWANNOTATIONCONTROL 2623
#define PEP_naSUBSETSTOLEGEND       2624
#define PEP_naLEGENDANNOTATIONTYPE  2625
#define PEP_szaLEGENDANNOTATIONTEXT 2626
#define PEP_dwaLEGENDANNOTATIONCOLOR 2627
#define PEP_nVERTSCROLLPOS           2628
#define PEP_bALLOWDEBUGOUTPUT       2629
#define PEP_szaMULTISUBTITLES       2630
#define PEP_szaMULTIBOTTOMTITLES    2631
#define PEP_bFOCALRECT              2632
#define PEP_fFONTSIZEGLOBALCNTL     2634
#define PEP_fFONTSIZETITLECNTL      2635
#define PEP_bSUBSETBYPOINT          2636
#define PEP_ptLASTMOUSEMOVE         2637
#define PEP_bALLOWOLEEXPORT         2638
#define PEP_dwDESKGRADIENTSTART     2687	
#define PEP_dwDESKGRADIENTEND       2688	
#define PEP_nDESKGRADIENTSTYLE      2689	
#define PEP_szDESKBMPFILENAME       2690	
#define PEP_nDESKBMPSTYLE           2691	
#define PEP_dwGRAPHGRADIENTSTART    2692	
#define PEP_dwGRAPHGRADIENTEND      2693	
#define PEP_nGRAPHGRADIENTSTYLE     2694
#define PEP_szGRAPHBMPFILENAME      2695
#define PEP_nGRAPHBMPSTYLE          2696
#define PEP_dwTABLEGRADIENTSTART    2697		
#define PEP_dwTABLEGRADIENTEND      2698
#define PEP_nTABLEGRADIENTSTYLE     2699
#define PEP_szTABLEBMPFILENAME      2700
#define PEP_nTABLEBMPSTYLE          2701
#define PEP_nDROPSHADOWOFFSETX      2679
#define PEP_nDROPSHADOWOFFSETY      2680
#define PEP_nDROPSHADOWSTEPS        2681
#define PEP_nDROPSHADOWWIDTH        2682
#define PEBG_TRANSPARENT            1
#define PEP_bPNGISTRANSPARENT       2683
#define PEP_dwPNGTRANSPARENTCOLOR   2684
#define PEP_bPNGISINTERLACED        2685
#define PEP_nJPGQUALITY             2686
#define PEP_nPRINTSTYLECONTROL      2705
#define PEP_nHIDEINTERSECTINGTEXT   2678 
#define PEP_bSTOP                   2677
#define PEP_nBITMAPGRADIENTMENU     2702
#define PEP_bBITMAPGRADIENTMODE     2703
#define PEP_bCONTROLBELONGSTOMAXDLG 2704
#define PEP_nLONGXAXISTICKMENU      2674
#define PEP_nLONGYAXISTICKMENU      2673
#define PEP_nQUICKSTYLE             2672
#define PEP_nQUICKSTYLEMENU         2671 
#define PEP_nVIEWINGSTYLEMENU       2640 
#define PEP_nFONTSIZEMENU           2641
#define PEP_nDATAPRECISIONMENU      2642
#define PEP_nDATASHADOWMENU         2643
#define PEP_bSEPARATORMENU          2654
#define PEP_nMAXIMIZEMENU           2655
#define PEP_nCUSTOMIZEDIALOGMENU    2656
#define PEP_nEXPORTDIALOGMENU       2657
#define PEP_nHELPMENU               2658
#define PEP_nBORDERTYPEMENU         2659
#define PEP_nSHOWLEGENDMENU         2660
#define PEP_nLEGENDLOCATIONMENU     2661
#define PEP_nSHOWTABLEANNOTATIONSMENU 2662
#define PEP_nMULTIAXISSTYLEMENU     2663
#define PEP_nFIXEDFONTMENU          2664
#define PEP_bSHOWALLTABLEANNOTATIONS 2665
#define PEP_bSHOWLEGEND             2666
#define PEP_naCUSTOMMENU            2667
#define PEP_naCUSTOMMENUSTATE       2668
#define PEP_naCUSTOMMENULOCATION    2669
#define PEP_szaCUSTOMMENUTEXT       2670
#define PEP_nLASTMENUINDEX          2675
#define PEP_nLASTSUBMENUINDEX       2676
#define PEP_faZDATA                 2900
#define PEP_bINVALID                2905
#define PEP_bOBJECTINSERVER         2910
#define PEP_hwndPARENTALCONTROL     2915
#define PEP_bOLDSCALINGLOGIC        2942
#define PEP_bFIXEDFONTS             2938
#define PEP_hSIZENSCURSOR           2939
#define PEP_dwHATCHBACKCOLOR        2941
#define PEP_naSUBSETHATCH           2940
#define PEP_nBORDERTYPES            2943
#define PEP_bDISABLECLIPPING        2944
#define PEP_fFONTSIZEMSCNTL         2945
#define PEP_fFONTSIZEMBCNTL         2946
#define PEP_fFONTSIZEGNCNTL         2947
#define PEP_fFONTSIZECPCNTL         2948
#define PEP_fFONTSIZEALCNTL         2949
#define PEP_nSBCODE                 2106
#define PEP_nSBPOS                  2107
#define PEP_nEXPORTTYPEDEF          2108
#define PEP_nEXPORTDESTDEF          2109
#define PEP_szEXPORTFILEDEF         2111
#define PEP_nEXPORTSIZEDEF          2112
#define PEP_szEXPORTUNITXDEF        2113
#define PEP_szEXPORTUNITYDEF        2114
#define PEP_fAXISTICKSPACING        3171
#define PEP_fXAXISNUMBERSPACING     3172
#define PEP_fXAXISTICKSPACING       3173
#define PEP_fGRAPHANNOTATIONSIZECNTL 3174
#define PEP_bSCROLLINGSCALECONTROL  3175
#define PEP_naSUBSETOBSTACLES       2323
#define PEP_nGRAPHANNOTMOVEABLE     2324
#define PEP_naGRAPHANNOTTEXTLOCATION 2321
#define PEP_nGRAPHANNOTTEXTDODGE    2322
#define PEP_nHSCROLLSTYLE           3331 
#define PEP_szSCALESYMBOLS          2986
#define PEP_nMOUSEWHEELFUNCTION     2127
#define PEP_rectIMAGEMAPPOLYS       2124
#define PEP_rectIMAGEMAPELLIPS      2126
#define PEP_nYAXISLINELIMIT         3183
#define PEP_nRYAXISLINELIMIT        3184
#define PEP_nXAXISLINELIMIT         3185
#define PEP_nTXAXISLINELIMIT        3186
#define PEP_bYAXISWHOLENUMBERS      3179
#define PEP_bRYAXISWHOLENUMBERS     3180
#define PEP_bXAXISWHOLENUMBERS      3181
#define PEP_bTXAXISWHOLENUMBERS     3182
#define PEP_bZAXISWHOLENUMBERS      4061
#define PEP_bDARKTEXTINSET          2123 
#define PEP_nGRADIENTBARS           3178 
#define PEP_bLINESHADOWS            3177 
#define PEP_nTEXTSHADOWS            2122 
#define PEP_nPOINTLABELROWS         3433
#define PEP_fFONTSIZETBCNTL         2121
#define PEP_hDESKBMPHANDLE          2117
#define PEP_hGRAPHBMPHANDLE         2118
#define PEP_hTABLEBMPHANDLE         2119
#define PEP_nMANUALSLICELABELLENGTH 3928
#define PEP_nMINTABLEFONTSIZE       2116
#define PEP_bALLOWGRIDNUMBERHOTSPOTSZ 4057
#define PEP_dwGRAPHANNOTBACKCOLOR   3176
#define PEP_bSIMPLELINELEGEND       2973
#define PEP_bSIMPLEPOINTLEGEND      2974
#define PEP_nWORKINGTABLE           2977
#define PEP_nTAROWS                 2951
#define PEP_nTACOLUMNS              2952
#define PEP_naTATYPE                2953
#define PEP_szaTATEXT               2954
#define PEP_dwaTACOLOR              2955
#define PEP_naTAHOTSPOT             2956
#define PEP_nTAHEADERROWS           2957
#define PEP_bTAHEADERCOLUMN         2958
#define PEP_naTACOLUMNWIDTH         2959
#define PEP_nTAHEADERORIENTATION    2960 
#define PEP_nTALOCATION             2961 
#define PEP_nTABORDER               2962 
#define PEP_dwTABACKCOLOR           2963 
#define PEP_dwTAFORECOLOR           2964 
#define PEP_nTATEXTSIZE             2965 
#define PEP_nTAAXISLOCATION         2966  
#define PEP_nTAGRIDLINECONTROL      2967  
#define PEP_bSHOWTABLEANNOTATION    2968 
#define PEP_naTAJUSTIFICATION       2969  
#define PEP_szTAFONT                2970 
#define PEP_szaTAFONTS              2971 
#define PEP_nDELIMITER              2950
#define PEP_bDISABLESYMBOLFIX       2972
#define PEP_bPAINTING               2916
#define PEP_hARROWCURSOR            2917
#define PEP_hZOOMCURSOR             2918
#define PEP_hHANDCURSOR             2919
#define PEP_hNODROPCURSOR           2920
#define PEP_bNOCUSTOMPARMS          2921
#define PEP_bNOHELP                 2922
#define PEP_szHELPFILENAME          2923
#define PEP_nLEGENDSTYLE            2975 
#define PEP_bNOSMARTTABLEPLACEMENT  2976 
#define PEP_nIMAGEADJUSTLEFT        2982   
#define PEP_nIMAGEADJUSTRIGHT       2983   
#define PEP_nIMAGEADJUSTTOP         2984   
#define PEP_nIMAGEADJUSTBOTTOM      2985   
#define PEP_bMODALDIALOGS           2978 
#define PEP_bMODELESSONTOP          2979
#define PEP_bMODELESSAUTOCLOSE      2980  
#define PEP_szSCALESYMBOLS          2986
#define PEP_nDIALOGRESULT           2981   
#define PEP_bALLOWTITLEHOTSPOTS     2924
#define PEP_bALLOWSUBTITLEHOTSPOTS  2925
#define PEP_bALLOWBOTTOMTITLEHOTSPOTS 2926
#define PEP_nCHARSET                2927
#define PEP_bALLOWJPEGOUTPUT        2928
#define PEP_bALLOWPAGE1             2930
#define PEP_bALLOWPAGE2             2931
#define PEP_bALLOWSUBSETSPAGE       2932
#define PEP_bALLOWPOINTSPAGE        2933
#define PEP_bALLOWFONTPAGE          2934
#define PEP_bALLOWCOLORPAGE         2935
#define PEP_bALLOWSTYLEPAGE         2936
#define PEP_bALLOWAXISPAGE          2937
#define PEP_szXAXISLABEL            3000
#define PEP_szYAXISLABEL            3005
#define PEP_nVBOUNDARYTYPES         3010
#define PEP_fUPPERBOUNDVALUE        3015
#define PEP_fLOWERBOUNDVALUE        3020
#define PEP_szUPPERBOUNDTEXT        3025
#define PEP_szLOWERBOUNDTEXT        3030
#define PEP_nINITIALSCALEFORYDATA   3035
#define PEP_nSCALEFORYDATA          3040
#define PEP_nYAXISSCALECONTROL      3045
#define PEP_nMANUALSCALECONTROLY    3050
#define PEP_fMANUALMINY             3055
#define PEP_fMANUALMAXY             3060
#define PEP_bNOSCROLLINGSUBSETCONTROL 3065
#define PEP_nSCROLLINGSUBSETS       3070
#define PEP_nCSCROLLINGSUBSETS      3075
#define PEP_naRANDOMSUBSETSTOGRAPH  3080
#define PEP_naCRANDOMSUBSETSTOGRAPH 3085
#define PEP_nPLOTTINGMETHOD         3090
#define PEP_nCPLOTTINGMETHOD        3095
#define PEP_nGRIDLINECONTROL        3100
#define PEP_nCGRIDLINECONTROL       3105
#define PEP_naPLOTTINGMETHODS 	    3103  
#define PEP_nSPEEDBOOST             3104  
#define PEP_nSHOWTICKMARKY          3106  
#define PEP_nSHOWTICKMARKRY         3107  
#define PEP_nSHOWTICKMARKX          3108
#define PEP_nOHLCMINWIDTH           3109  
#define PEP_nMULTIAXESSIZING        3111
#define PEP_naPOINTHATCH            3114
#define PEP_bYAXISVERTGRIDNUMBERS   3113
#define PEP_bDAYLIGHTSAVINGS        3112
#define PEP_dwBARBORDERCOLOR        3116
#define PEP_fLEFTEDGESPACING        3117
#define PEP_fRIGHTEDGESPACING       3118
#define PEP_fAXISNUMBERSPACING      3119
#define PEP_fAXISTICKSPACING		3171
#define PEP_fXAXISNUMBERSPACING     3172
#define PEP_fXAXISTICKSPACING		3173
#define PEP_fGRAPHANNOTATIONSIZECNTL 3174
#define PEP_bSCROLLINGSCALECONTROL  3175
#define PEP_bTRIANGLEANNOTATIONADJ  3126
#define PEP_fGRIDASPECT             3124
#define PEP_faGRIDHOTSPOTVALUE      3123
#define PEP_bVGNAXISLABELLOCATION   3121
#define PEP_bALLOWGRIDNUMBERHOTSPOTSY 3122
#define PEP_structSPRINGDAYLIGHT    3127
#define PEP_structFALLDAYLIGHT      3128
#define PEP_bDATETIMESHOWSECONDS    3129
#define PEP_faWORKINGAXESPROPORTIONS 3131
#define PEP_bFIXEDLINETHICKNESS     3140
#define PEP_bFIXEDSPMWIDTH          3141
#define PEP_fDASHLINETHICKNESS      3142
#define PEP_naHORZLINEANNOTHOTSPOT  3138
#define PEP_naVERTLINEANNOTHOTSPOT  3139
#define PEP_nYEARMONTHDAYPROMPT     3133
#define PEP_nTIMELABELTYPE          3134
#define PEP_nDAYLABELTYPE           3135
#define PEP_nMONTHLABELTYPE         3136
#define PEP_nYEARLABELTYPE          3137
#define PEP_dwaAPPENDPOINTCOLORS    3132 
#define PEP_bGRIDINFRONT            3110
#define PEP_bCGRIDINFRONT           3115
#define PEP_nAXISSIZEY              3143
#define PEP_nAXISLOCATIONY          3144
#define PEP_nAXISSIZERY             3145
#define PEP_nAXISLOCATIONRY         3146
#define PEP_bTREATCOMPSASNORMAL     3120
#define PEP_bCTREATCOMPSASNORMAL    3125
#define PEP_nCOMPARISONSUBSETS      3130
#define PEP_bDISABLESORTPLOTMETHODS 3147 
#define PEP_nMULTIAXISSTYLE         3149 
#define PEP_bFLOATINGBARS           3151
#define PEP_nMULTIAXISSEPARATORSIZE 3153   
#define PEP_nCURSORPROMPTLOCATION   3152   
#define PEP_nTICKSTYLE              3158   
#define PEP_dwTICKCOLOR             3159   
#define PEP_naPOINTTYPES            3156   
#define PEP_naSUBSETFORPOINTTYPES   3157   
#define PEP_naSUBSETFORPOINTCOLORS  3155   
#define PEP_nZOOMSTYLE              3154   
#define PEP_structCUSTOMGRIDNUMBERS 3162   
#define PEP_bCUSTOMGRIDNUMBERSY     3160   
#define PEP_bCUSTOMGRIDNUMBERSRY    3161   
#define PEP_bCUSTOMGRIDNUMBERSX     3163   
#define PEP_nGRIDLINEMENU           3164
#define PEP_nPLOTMETHODMENU         3165
#define PEP_nGRIDINFRONTMENU        3166
#define PEP_nTREATCOMPARISONSMENU   3167
#define PEP_nMARKDATAPOINTSMENU     3168
#define PEP_nSHOWANNOTATIONSMENU    3169
#define PEP_nUNDOZOOMMENU           3170
#define PEP_bALLOWCOORDPROMPTING    3200
#define PEP_bALLOWGRAPHHOTSPOTS     3205
#define PEP_bALLOWDATAHOTSPOTS      3210
#define PEP_bMARKDATAPOINTS         3215
#define PEP_bCMARKDATAPOINTS        3220
#define PEP_nRYAXISCOMPARISONSUBSETS 3225
#define PEP_nRYAXISSCALECONTROL     3230
#define PEP_nINITIALSCALEFORRYDATA  3235
#define PEP_nMANUALSCALECONTROLRY   3240
#define PEP_fMANUALMINRY            3245
#define PEP_fMANUALMAXRY            3250
#define PEP_szRYAXISLABEL           3255
#define PEP_nSCALEFORRYDATA         3256
#define PEP_bALLOWPLOTCUSTOMIZATION 3260
#define PEP_bNEGATIVEFROMXAXIS      3261
#define PEP_bMANUALYAXISTICKNLINE   3262
#define PEP_fMANUALYAXISTICK        3263
#define PEP_fMANUALYAXISLINE        3264
#define PEP_bMANUALRYAXISTICKNLINE  3265
#define PEP_fMANUALRYAXISTICK       3266
#define PEP_fMANUALRYAXISLINE       3267
#define PEP_fNULLDATAVALUE          3268
#define PEP_nPOINTSIZE              3269
#define PEP_naSUBSETPOINTTYPES      3270
#define PEP_naSUBSETLINETYPES       3271
#define PEP_bALLOWBESTFITCURVE      3272
#define PEP_nBESTFITDEGREE          3273
#define PEP_bALLOWSPLINE            3274
#define PEP_nCURVEGRANULARITY       3275
#define PEP_faAPPENDYDATA           3276
#define PEP_szaAPPENDPOINTLABELDATA 3277
#define PEP_bALLOWLINE              3279
#define PEP_bALLOWPOINT             3280
#define PEP_bALLOWBESTFITLINE       3281
#define PEP_nALLOWZOOMING           3282
#define PEP_bZOOMMODE               3283
#define PEP_fZOOMMINY               3284
#define PEP_fZOOMMAXY               3285
#define PEP_bFORCERIGHTYAXIS        3286
#define PEP_bALLOWPOINTSPLUSLINE    3287
#define PEP_bALLOWPOINTSPLUSSPLINE  3288
#define PEP_nSYMBOLFREQUENCY        3289
#define PEP_bSHOWANNOTATIONS        3290
#define PEP_bCSHOWANNOTATIONS       3202
#define PEP_dwANNOTATIONCOLOR       3203
#define PEP_dwCANNOTATIONCOLOR      3204
#define PEP_faGRAPHANNOTATIONX      3291
#define PEP_faGRAPHANNOTATIONY      3292
#define PEP_szaGRAPHANNOTATIONTEXT  3293
#define PEP_nMAXAXISANNOTATIONCLUSTER 3296
#define PEP_faXAXISANNOTATION       3297
#define PEP_szaXAXISANNOTATIONTEXT  3298
#define PEP_faYAXISANNOTATION       3299
#define PEP_szaYAXISANNOTATIONTEXT  3201
#define PEP_bANNOTATIONSINFRONT     3208
#define PEP_nCURSORPAGEAMOUNT       3211
#define PEP_fLINEGAPTHRESHOLD       3212
#define PEP_faHORZLINEANNOTATION    3213
#define PEP_szaHORZLINEANNOTATIONTEXT 3214
#define PEP_naHORZLINEANNOTATIONTYPE 3216
#define PEP_dwaHORZLINEANNOTATIONCOLOR 3217
#define PEP_faVERTLINEANNOTATION    3218
#define PEP_szaVERTLINEANNOTATIONTEXT 3219
#define PEP_naVERTLINEANNOTATIONTYPE 3221
#define PEP_dwaVERTLINEANNOTATIONCOLOR 3222
#define PEP_bSHOWGRAPHANNOTATIONS   3223
#define PEP_bSHOWXAXISANNOTATIONS   3224
#define PEP_bSHOWYAXISANNOTATIONS   3226
#define PEP_bSHOWHORZLINEANNOTATIONS 3227
#define PEP_bSHOWVERTLINEANNOTATIONS 3228
#define PEP_bALLOWGRAPHANNOTHOTSPOTS 3229
#define PEP_bALLOWXAXISANNOTHOTSPOTS 3231
#define PEP_bALLOWYAXISANNOTHOTSPOTS 3232
#define PEP_bALLOWHORZLINEANNOTHOTSPOTS 3233
#define PEP_bALLOWVERTLINEANNOTHOTSPOTS 3234
#define PEP_dwaGRAPHANNOTATIONCOLOR 3236
#define PEP_dwaXAXISANNOTATIONCOLOR 3237
#define PEP_dwaYAXISANNOTATIONCOLOR 3238
#define PEP_nGRAPHANNOTATIONTEXTSIZE 3242
#define PEP_nAXESANNOTATIONTEXTSIZE 3243
#define PEP_nLINEANNOTATIONTEXTSIZE 3244
#define PEP_naGRAPHANNOTATIONTYPE   3246
#define PEP_nZOOMINTERFACEONLY      3247
#define PEP_fZOOMMINX               3248
#define PEP_fZOOMMAXX               3249
#define PEP_nDATAHOTSPOTLIMIT       3251
#define PEP_nHOURGLASSTHRESHOLD     3252
#define PEP_nHORZSCROLLPOS          3253
#define PEP_bALLOWAREA              3254
#define PEP_bVERTORIENT90DEGREES    3257
#define PEP_dwaPOINTCOLORS          3258
#define PEP_naMULTIAXESSUBSETS      3001
#define PEP_naGRAPHANNOTATIONAXIS   3002
#define PEP_naHORZLINEANNOTATIONAXIS 3003
#define PEP_naYAXISANNOTATIONAXIS   3004
#define PEP_nWORKINGAXIS            3006
#define PEP_faMULTIAXESPROPORTIONS  3007
#define PEP_naLEGENDANNOTATIONAXIS  3008
#define PEP_bLOGSCALEEXPLABELS      3009
#define PEP_nPLOTTINGMETHODII       3011
#define PEP_nCPLOTTINGMETHODII      3012
#define PEP_faXDATAII               3013
#define PEP_faYDATAII               3014
#define PEP_bUSINGXDATAII           3016
#define PEP_bUSINGYDATAII           3017
#define PEP_nDATETIMEMODE           3018
#define PEP_fBARWIDTH               3019
#define PEP_nSPECIFICPLOTMODE       3021
#define PEP_bALLOWBAR               3022
#define PEP_structGRAPHLOC          3023
#define PEP_faAPPENDYDATAII         3024
#define PEP_bYAXISONRIGHT           3026
#define PEP_nSHOWYAXIS              3027
#define PEP_nSHOWRYAXIS             3028
#define PEP_nSHOWXAXIS              3029
#define PEP_nGRIDSTYLE              3032
#define PEP_bINVERTEDYAXIS          3033
#define PEP_bINVERTEDRYAXIS         3034
#define PEP_dwYAXISCOLOR            3036
#define PEP_dwRYAXISCOLOR           3037
#define PEP_dwXAXISCOLOR            3038                
#define PEP_fFONTSIZEAXISCNTL       3041
#define PEP_fFONTSIZELEGENDCNTL     3042
#define PEP_bYAXISLONGTICKS         3043
#define PEP_bRYAXISLONGTICKS        3044
#define PEP_nMULTIAXESSEPARATORS    3046
#define PEP_nZOOMMINAXIS            3047
#define PEP_nZOOMMAXAXIS            3048
#define PEP_rectGRAPH               3049
#define PEP_rectAXIS                3051
#define PEP_szLEFTMARGIN            3052
#define PEP_szTOPMARGIN             3053
#define PEP_szRIGHTMARGIN           3054
#define PEP_szBOTTOMMARGIN          3056
#define PEP_bAUTOSCALEDATA          3057                                        
#define PEP_faBESTFITCOEFFS         3058
#define PEP_naOVERLAPMULTIAXES      3059
#define PEP_bNOHIDDENLINESINAREA    3061
#define PEP_bSPECIFICPLOTMODECOLOR  3062
#define PEP_nAUTOMINMAXPADDING      3063
#define PEP_nLOGICALLIMIT           3064
#define PEP_bNULLDATAGAPS           3066
#define PEP_bALLOWSTEP              3067
#define PEP_naSUBSETDEGREE          3068
#define PEP_bSCROLLINGVERTZOOM      3069
#define PEP_szAXISFORMATY           3071
#define PEP_szAXISFORMATRY          3072
#define PEP_fZOOMMINRY              3073
#define PEP_fZOOMMAXRY              3074
#define PEP_n3DTHRESHOLD            3076
#define PEP_bXAXISLONGTICKS         3078
#define PEP_bTXAXISLONGTICKS        3079
#define PEP_nHOTSPOTSIZE            3081
#define PEP_nLEGENDLOCATION         3082
#define PEP_bALLOWAXISLABELHOTSPOTS 3083
#define PEP_bALLOWAXISHOTSPOTS      3084
#define PEP_bAPPENDWITHNOUPDATE     3086
#define PEP_bBESTFITFIX             3087
#define PEP_dwBOXPLOTCOLOR          3088
#define PEP_naGRAPHANNOTATIONHOTSPOT 3089
#define PEP_bALLOWRIBBON            3091
#define PEP_bNOGRIDLINEMULTIPLES    3092
#define PEP_nSPECIALSCALINGY        3093
#define PEP_nSPECIALSCALINGRY       3094
#define PEP_nDELTAX                 3096
#define PEP_nDELTASPERDAY           3097
#define PEP_fSTARTTIME              3098
#define PEP_fENDTIME                3099
#define PEP_nLOGTICKTHRESHOLD       3101
#define PEP_nMINIMUMPOINTSIZE       3102
#define PEP_naAUTOSTATSUBSETS       3300    
#define PEP_bNOSTACKEDDATA          3305
#define PEP_nPOINTSTOGRAPHINIT      3310
#define PEP_nPOINTSTOGRAPHVERSION   3315
#define PEP_nCPOINTSTOGRAPHVERSION  3320
#define PEP_nPOINTSTOGRAPH          3325
#define PEP_nCPOINTSTOGRAPH         3330
#define PEP_nHSCROLLSTYLE           3331 
#define PEP_naRANDOMPOINTSTOGRAPH   3335
#define PEP_naCRANDOMPOINTSTOGRAPH  3340
#define PEP_nFORCEVERTICALPOINTS    3345
#define PEP_nCFORCEVERTICALPOINTS   3350
#define PEP_nGRAPHPLUSTABLE         3355
#define PEP_nCGRAPHPLUSTABLE        3360
#define PEP_nTABLEWHAT              3365
#define PEP_nCTABLEWHAT             3370
#define PEP_bALLOWTABLEHOTSPOTS     3400
#define PEP_bALLOWHISTOGRAM         3401
#define PEP_naALTFREQUENCIES        3403
#define PEP_nTARGETPOINTSTOTABLE    3404
#define PEP_nALTFREQTHRESHOLD       3405
#define PEP_fMANUALSTACKEDMAXY      3406
#define PEP_nMAXPOINTSTOGRAPH       3407
#define PEP_bNORANDOMPOINTSTOGRAPH  3408
#define PEP_szMANUALMAXPOINTLABEL   3409
#define PEP_szMANUALMAXDATASTRING   3410
#define PEP_bALLOWBESTFITLINEII     3413
#define PEP_bALLOWBESTFITCURVEII    3414
#define PEP_bAPPENDTOEND            3415
#define PEP_bALLOWHORIZONTALBAR     3416
#define PEP_bFLOATINGSTACKEDBARS    3424
#define PEP_nSCROLLINGRANGE         3425
#define PEP_nSCROLLINGFACTOR        3426
#define PEP_nFIRSTPTLABELOFFSET     3417
#define PEP_fMANUALSTACKEDMINY      3418
#define PEP_bALLOWHORZBARSTACKED    3419
#define PEP_bTABLECOMPARISONSUBSETS 3420
#define PEP_nGRAPHPLUSTABLEMENU     3430
#define PEP_nFORCEVERTPOINTSMENU    3431
#define PEP_nTABLEWHATMENU          3432
#define PEP_fPOINTPADDING           3427   
#define PEP_fPOINTPADDINGAREA       3428   
#define PEP_fPOINTPADDINGBAR        3429   
#define PEP_bFORMATTABLE            3421
#define PEP_bALLOWTABLE             3422
#define PEP_nAUTOXDATA              3423
#define PEP_nINCLUDEDATALABELSMENU  3696
#define PEP_fZOOMMINTX              3697
#define PEP_fZOOMMAXTX              3698
#define PEP_nINITIALSCALEFORXDATA   3600
#define PEP_nSCALEFORXDATA          3605
#define PEP_nXAXISSCALECONTROL      3610
#define PEP_nSHOWTICKMARKTX         3689
#define PEP_bCONTOURSTYLELEGEND     3690
#define PEP_szaCONTOURLABELS        3691
#define PEP_bALLOWGRIDNUMBERHOTSPOTSX 3692
#define PEP_structEXTRAAXISX        3693
#define PEP_structEXTRAAXISTX       3694
#define PEP_nMANUALSCALECONTROLX    3615
#define PEP_fMANUALMINX             3620
#define PEP_fMANUALMAXX             3625
#define PEP_bGRAPHDATALABELS        3630
#define PEP_bCGRAPHDATALABELS       3635
#define PEP_bCUSTOMGRIDNUMBERSTX    3695   
#define PEP_bALLOWBUBBLE            3640
#define PEP_nBUBBLESIZE             3641
#define PEP_nALLOWDATALABELS        3642
#define PEP_szaDATAPOINTLABELS      3643                                   
#define PEP_bMANUALXAXISTICKNLINE   3644
#define PEP_fMANUALXAXISTICK        3645
#define PEP_fMANUALXAXISLINE        3646
#define PEP_bALLOWSTICK             3648
#define PEP_bSCROLLINGHORZZOOM      3652
#define PEP_bNORANDOMPOINTSTOEXPORT 3653
#define PEP_bXAXISVERTNUMBERING     3654
#define PEP_bENGSTATIONFORMAT       3655
#define PEP_fNULLDATAVALUEX         3656
#define PEP_bASSUMESEQDATA          3657
#define PEP_faAPPENDXDATA           3658
#define PEP_faAPPENDXDATAII         3659
#define PEP_nTXAXISCOMPARISONSUBSETS 3661
#define PEP_nTXAXISSCALECONTROL     3662
#define PEP_nINITIALSCALEFORTXDATA  3663
#define PEP_nMANUALSCALECONTROLTX   3664
#define PEP_fMANUALMINTX            3665
#define PEP_fMANUALMAXTX            3666
#define PEP_szTXAXISLABEL           3667
#define PEP_nSCALEFORTXDATA         3668
#define PEP_bMANUALTXAXISTICKNLINE  3669
#define PEP_fMANUALTXAXISTICK       3670
#define PEP_fMANUALTXAXISLINE       3671
#define PEP_bFORCETOPXAXIS          3672
#define PEP_bXAXISONTOP             3673
#define PEP_bINVERTEDXAXIS          3674 
#define PEP_bINVERTEDTXAXIS         3675
#define PEP_nSHOWTXAXIS             3676
#define PEP_dwTXAXISCOLOR           3677                
#define PEP_szAXISFORMATX           3678
#define PEP_szAXISFORMATTX          3679
#define PEP_bALLOWCONTOURLINES      3680
#define PEP_bALLOWCONTOURCOLORS     3681
#define PEP_fMANUALMINZ             3682
#define PEP_fMANUALMAXZ             3683
#define PEP_nMANUALSCALECONTROLZ    3684
#define PEP_fMANUALZAXISLINE        3685
#define PEP_nCONTOURLINELABELDENSITY 3686
#define PEP_bSPECIALDATETIMEMODE    3687
#define PEP_bSMITHCHART             3800
#define PEP_nSMITHCHART             3800
#define PEP_bSMARTGRID              3801
#define PEP_nSHOWPOLARGRID          3802
#define PEP_nZERODEGREEOFFSET       3803
#define PEP_fPOLARTICKTHRESHOLD     3804
#define PEP_fPOLARLINETHRESHOLD     3805
#define PEP_fPOLAR30DEGTHRESHOLD    3806
#define PEP_nGROUPINGPERCENT        3900
#define PEP_nCGROUPINGPERCENT       3905
#define PEP_nDATALABELTYPE          3910
#define PEP_nCDATALABELTYPE         3915
#define PEP_nAUTOEXPLODE            3920
#define PEP_bDISABLE3DSHADOW        3927
#define PEP_nPERCENTORVALUEMENU     3925
#define PEP_nGROUPPERCENTMENU       3926
#define PEP_nSHOWPIELABELS          3921   
#define PEP_bSHOWPIELEGEND          3922   
#define PEP_nSLICEHATCHING          3923   
#define PEP_nSLICESTARTLOCATION     3924   
#define PEP_szZAXISLABEL            4000
#define PEP_nDEGREEOFROTATION       4001
#define PEP_bALLOWROTATION          4002
#define PEP_bAUTOROTATION           4003
#define PEP_nROTATIONINCREMENT      4004
#define PEP_nROTATIONDETAIL         4005
#define PEP_bALLOWHORZSCROLLBAR     4006
#define PEP_bALLOWVERTSCROLLBAR     4007
#define PEP_nVIEWINGHEIGHT          4008
#define PEP_bSHADEDPOLYGONBORDERS   4056
#define PEP_nSHOWBOUNDINGBOXMENU    4058
#define PEP_nROTATIONMENU           4059
#define PEP_nCONTOURMENU            4060
#define PEP_bSURFACEPOLYGONBORDERS  4009
#define PEP_bNOSURFACEPOLYGONBORDERS 4009
#define PEP_nSHOWBOUNDINGBOX        4010
#define PEP_nROTATIONSPEED          4011
#define PEP_bADDSKIRTS              4012
#define PEP_nPOLYMODE               4013
#define PEP_structPOLYDATA          4014
#define PEP_dwXZBACKCOLOR           4015
#define PEP_dwYBACKCOLOR            4016
#define PEP_dwZAXISCOLOR            4017
#define PEP_nSHOWZAXIS              4018
#define PEP_bMANUALZAXISTICKNLINE   4019
#define PEP_fMANUALZAXISTICK        4020
#define PEP_bZAXISLONGTICKS         4021
#define PEP_fZDISTANCE              4022
#define PEP_bINVERTEDZAXIS          4023
#define PEP_bCUSTOMGRIDNUMBERSZ     4055   
#define PEP_nSHOWCONTOUR            4024
#define PEP_bALLOWCONTOURCONTROL    4025
#define PEP_bSHOWCONTOURLEGENDS     4026
#define PEP_fMANUALCONTOURLINE      4027
#define PEP_fMANUALCONTOURMIN       4028
#define PEP_fMANUALCONTOURMAX       4029
#define PEP_nMANUALCONTOURSCALECONTROL 4030
#define PEP_nSHADINGSTYLE           4031
#define PEP_bRESETGDICACHE          4032
#define PEP_bSHOWZAXISLINEANNOTATIONS 4035
#define PEP_faZAXISLINEANNOTATION 4036
#define PEP_szaZAXISLINEANNOTATIONTEXT 4037
#define PEP_naZAXISLINEANNOTATIONTYPE 4038
#define PEP_dwaZAXISLINEANNOTATIONCOLOR 4039
#define PEP_faGRAPHANNOTATIONZ      4040
#define PEP_bANNOTATIONSONSURFACES  4041
#define PEP_bALLOWWIREFRAME         4042
#define PEP_bALLOWSURFACE           4043
#define PEP_bALLOWSURFACESHADING    4044
#define PEP_bALLOWSURFACECONTOUR    4045
#define PEP_bALLOWSURFACEPIXEL      4046
#define PEP_bUSINGZDATAII           4047
#define PEP_faZDATAII               4048
#define PEP_faAPPENDZDATA           4049
#define PEP_fNULLDATAVALUEZ         4050
#define PEP_nINITIALSCALEFORZDATA   4051
#define PEP_nSCALEFORZDATA          4052
#define PEP_faAPPENDZDATAII         4053
#define PEP_bDEGREEPROMPTING        4054

typedef struct 
{
	float 		X;
	float 		Y;
	float 		Z;
} Point3D;

typedef struct
{
	Point3D 	Vertices[4];
	int 		NumberOfVertices;
	DWORD 		PolyColor;
	DWORD		dwReserved1;
	DWORD 		dwReserved2; 
} PolygonData; // 64 byte structure

typedef struct 
{
	WORD	nMajVersion;	// ProEssentials version number
	WORD	nMinVersion;
	DWORD 	nObjectType;	 
	DWORD 	dwSize;
	DWORD	extra[8];
} PEFILEHDR;

// Notification Messages //
#define PEWN_CURSORMOVE        (WM_USER + 2929)
#define PEWN_CLICKED           (WM_USER + 2930)
#define PEWN_DBLCLICKED        (WM_USER + 2931)
#define PEWN_SETFOCUS          (WM_USER + 2932)
#define PEWN_KILLFOCUS         (WM_USER + 2933)
#define PEWN_CHANGINGPARMS     (WM_USER + 2937)
#define PEWN_ZOOMIN            (WM_USER + 2942)
#define PEWN_ZOOMOUT           (WM_USER + 2943)
#define PEWN_RBUTTONCLK        (WM_USER + 2944)
#define PEWN_MOUSEMOVE         (WM_USER + 2945)
#define PEWN_RBUTTONDBLCLK     (WM_USER + 2946)
#define PEWN_LBUTTONUP         (WM_USER + 2947)
#define PEWN_RBUTTONUP         (WM_USER + 2948)
#define PEWN_PRECURSORMOVE     (WM_USER + 2949)
#define PEWN_CUSTOMIZEDLG      (WM_USER + 2950)
#define PEWN_POPUPMENU         (WM_USER + 2951)
#define PEWN_KEYDOWN           (WM_USER + 2952)
#define PEWN_MULTIAXESSIZE     (WM_USER + 2953)
#define PEWN_MULTIAXESSIZECHANGE (WM_USER + 2954)
#define PEWN_CUSTOMGRIDNUMBERS (WM_USER + 2955)
#define PEWN_CUSTOMMENU        (WM_USER + 2956)
#define PEWM_PARENTVIEW_RESIZED (WM_USER + 2934)
#define PEWM_INVALIDATERECT    (WM_USER + 2938)
#define PEWM_FSCB_ADDSTRING    (WM_USER + 2939)
#define PEWM_DIRLISTNOTIFY     (WM_USER + 2940)
#define PEWM_ABORTDIALOGNEXTPAGE (WM_USER + 2941)
#define PEWN_PREHSCROLL        (WM_USER + 2957)
#define PEWN_PREVSCROLL        (WM_USER + 2958)


#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#define DllExport __declspec( dllexport ) 
#define DllExportReturnBool __declspec( dllexport ) BOOL __stdcall
#define DllExportReturnHWnd __declspec( dllexport ) HWND __stdcall
#define DllExportReturnInt __declspec( dllexport ) int __stdcall
#define DllExportReturnWord __declspec( dllexport ) WORD __stdcall
#define DllExportReturnVoid __declspec( dllexport ) void __stdcall
#define DllExportReturnHMeta __declspec( dllexport ) HMETAFILE __stdcall
#define DllExportReturnLong __declspec( dllexport ) long __stdcall
#define DllExportReturnDWord __declspec( dllexport ) DWORD __stdcall

DllExportReturnBool PEdrawcursor(HWND hObject, HDC hdc, UINT nAction);
DllExportReturnBool PEdrawtable(HWND hObject, UINT nTAIndex, HDC hdc);
DllExportReturnBool PEvset(HWND hObject, UINT nProperty, void FAR* lpvData, int nElements);
DllExportReturnInt PEvget(HWND hObject, UINT nProperty, void FAR* lpvDest);
DllExportReturnBool PEvsetcell(HWND hObject, UINT nProperty, int nCell, void FAR* lpvData);
DllExportReturnBool PEvgetcell(HWND hObject, UINT nProperty, int nCell, void FAR* lpvDest);
DllExportReturnBool PEvsetcellEx(HWND hObject, UINT nProperty, int nSubset, int nPoint, void FAR* lpvData);
DllExportReturnBool PEvgetcellEx(HWND hObject, UINT nProperty, int nSubset, int nPoint, void FAR* lpvDest);
DllExportReturnBool PEnset(HWND hObject, UINT nProperty, int nData);
DllExportReturnInt PEnget(HWND hObject, UINT nProperty);
DllExportReturnBool PEszset(HWND hObject, UINT nProperty, char FAR* szData);
DllExportReturnBool PEszget(HWND hObject, UINT nProperty, char FAR* szData);
DllExportReturnBool PEgetglobal(HWND hObject, GLOBALPROPERTIES FAR* lpData);
DllExportReturnBool PEsetglobal(HWND hObject, GLOBALPROPERTIES FAR* lpData);
DllExportReturnBool PEgetgraph(HWND hObject, GRAPHPROPERTIES FAR* lpData);
DllExportReturnBool PEsetgraph(HWND hObject, GRAPHPROPERTIES FAR* lpData);
DllExportReturnBool PEgetsgraph(HWND hObject, SGRAPHPROPERTIES FAR* lpData);
DllExportReturnBool PEsetsgraph(HWND hObject, SGRAPHPROPERTIES FAR* lpData);
DllExportReturnBool PEgetpiechart(HWND hObject, PIEPROPERTIES FAR* lpData);
DllExportReturnBool PEsetpiechart(HWND hObject, PIEPROPERTIES FAR* lpData);
DllExportReturnHWnd PEcreate(UINT nObjectType, DWORD dwStyle, RECT FAR* lpRect, HWND hParent, UINT nID);
DllExportReturnBool PEdestroy(HWND hObject);
DllExportReturnBool PEload(HWND hObject, HGLOBAL FAR* lphGlbl);
DllExportReturnBool PEstore(HWND hObject, HGLOBAL FAR* lphGlbl, DWORD FAR* lpdwSize);
DllExportReturnBool PEloadpartial(HWND hObject, HGLOBAL FAR* lphGlbl);
DllExportReturnBool PEstorepartial(HWND hObject, HGLOBAL FAR* lphGlbl, DWORD FAR* lpdwSize);
DllExportReturnBool PEsavetofile ( HWND hObject, char FAR* lpFileName );	// jrw 12/11/98
DllExportReturnBool PEloadfromfile( HWND hObject, char FAR* lpFileName );
DllExportReturnHWnd PEcreatefromfile( char FAR* lpFileName, HWND hParent, RECT FAR* lpRect, UINT nID );
DllExportReturnHMeta PEgetmeta(HWND hObject);
DllExportReturnBool PEresetimage(HWND hObject, UINT nLength, UINT nHeight);
DllExportReturnBool PEresetimageEx(HWND hObject, UINT nExtX, UINT nExtY, UINT nOrgX, UINT nOrgY);
DllExportReturnInt PElaunchcustomize(HWND hObject);
DllExportReturnInt PElaunchcustomizeEx(HWND hObject, UINT nPageID);
DllExportReturnBool PElaunchexport(HWND hObject);
DllExportReturnBool PElaunchmaximize(HWND hObject);
DllExportReturnBool PElaunchtextexport(HWND hObject, BOOL bToFile, char FAR* lpszFilename);
DllExportReturnBool PElaunchprintdialog(HWND hObject, BOOL bFullPage, POINT FAR* pSize);
DllExportReturnInt PElaunchcolordialog(HWND hObject);
DllExportReturnInt PElaunchfontdialog(HWND hObject);
DllExportReturnBool PElaunchpopupmenu(HWND hObject, POINT FAR* pLocation);
DllExportReturnBool PEreinitialize(HWND hObject);
DllExportReturnBool PEreinitializecustoms(HWND hObject);
DllExportReturnBool PEreset(HWND hObject);
DllExportReturnDWord PEgethelpcontext(HWND hWnd);
DllExportReturnBool PEcopymetatoclipboard(HWND hObject, POINT FAR* lpPoint);
DllExportReturnBool PEcopymetatofile(HWND hObject, POINT FAR* lpPoint, char FAR* lpszFilename);
DllExportReturnBool PEcopybitmaptoclipboard(HWND hObject, POINT FAR* lpPoint);
DllExportReturnBool PEcopybitmaptofile(HWND hObject, POINT FAR* lpPoint, char FAR* lpszFilename);
DllExportReturnBool PEcopyjpegtoclipboard(HWND hObject, POINT FAR* lpPoint);
DllExportReturnBool PEcopyjpegtofile(HWND hObject, POINT FAR* lpPoint, char FAR* lpszFilename);
DllExportReturnDWord PEcopyjpegtohglobal(HWND hObject, POINT FAR* lpPoint, HGLOBAL* pHG);
DllExportReturnBool PEcopypngtoclipboard(HWND hObject, POINT FAR* lpPoint);
DllExportReturnBool PEcopypngtofile(HWND hObject, POINT FAR* lpPoint, char FAR* lpszFilename);
DllExportReturnDWord PEcopypngtohglobal(HWND hObject, POINT FAR* lpPoint, HGLOBAL* pHG);
DllExportReturnBool PEcopyoletoclipboard(HWND hObject, POINT FAR* lpPoint);
DllExportReturnBool PEbitmapandgradients(HWND hObject, HDC hTargetDC, HDC hMemDC, LPRECT pr);
DllExportReturnBool PEallocateindmemory(HWND hObject);
DllExportReturnBool PEfilterdllmsg(LPMSG lpMsg);
DllExportReturnVoid PEprocessdllidle();
DllExportReturnVoid PEDebugOutput(UINT nId);
DllExportReturnBool PEValidateProperty(UINT nCntlType, UINT nProperty);
DllExportReturnVoid PEcreateserialdate(double* pfSerial, TM* dt, int nType);
DllExportReturnVoid PEdecipherserialdate(double* pfSerial, TM* dt, int nType);
DllExportReturnBool PEprintgraph(HWND hObject, int hDC, int nWidth, int nHeight, int nOrient);
DllExportReturnBool PEconvpixeltograph(HWND hObject, int* pnAxis, int* pnX, int* pnY, double* pfX, double* pfY, BOOL bRightAxis, BOOL bTopAxis, BOOL bViceVersa);
DllExportReturnBool PEgethotspot(HWND hObject, int nX, int nY);
DllExportReturnBool PEpartialresetimage(HWND hObject, int nStartPoint, int nPointsToAdd);
DllExportReturnBool PEvsetEx(HWND hObject, UINT property, int nStartingCell, int nCellCount, void FAR* lpData, void FAR* lpMemSetValue);
DllExportReturnBool PEvgetEx(HWND hObject, UINT property, int nStartingCell, int nCellCount, void FAR* lpData );
DllExportReturnBool PEserializetofile(HWND hObject, char FAR* lpszFilename);


#ifdef __cplusplus
}
#endif

#endif  // __PEGRPAPI_H__

