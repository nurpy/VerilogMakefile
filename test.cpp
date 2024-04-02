        /*******************************************************************************************
*
*   raylib [text] example - Input Box
*
*   Example originally created with raylib 1.7, last time updated with raylib 3.5
*
*   Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
*   BSD-like license that allows static linking with closed source software
*
*   Copyright (c) 2017-2022 Ramon Santamaria (@raysan5)
*
********************************************************************************************/
#include "raylib.h"

#include <iostream>
#include <vector>
#include <fstream>
//#include "test.hpp"


#define MAX_INPUT_CHARS     19
#define MAX_INPUT_CHARS_BUSSIZE 19
const int screenWidth = 800;
const int screenHeight = 450;



struct field{
	char name[MAX_INPUT_CHARS + 1];// = "\0";      // NOTE: One extra space required for null terminator char '\0'
    char BusSizeVal[MAX_INPUT_CHARS_BUSSIZE+1];
    int letterCount;// = 0;
    int letterCountBusSize;
	 Rectangle BusOrNot;
	 Rectangle BusSize;
	 Rectangle BigEndianLittle;
	 Rectangle inputOutput;
    Rectangle textBox;// = { screenWidth/2.0f-225/2, 180, 225, 50 };
    Rectangle parameterOrNot;
    
    
	
	bool mouseOnText[6];
    bool textClicked[6];
    bool higlightSection[6];
    int stateSelected[4];
    int state4;
    int cursor;
};
typedef struct field field;

struct fieldManager{
    std::vector<field>* allFields;
    float bottomHeight;
    float topHeight;
    float currentPos;
    float shiftFields;
    bool menuBarSelected;
    bool menuClosed;
    std::vector<std::vector<int>> * states;

};
typedef struct fieldManager fieldManager;

struct cursorBar{
    float heightposition;
    float length;

};
typedef struct cursorBar cursorBar;


std::string MakeStringTestbench(fieldManager* fields,std::string moduleName){

std::string parameters = "";
    std::string ports = "";
    std::string paraminputs;
    std::string portinputs;
    for(int i=0; i< fields->allFields->size();i++){
        if((*(fields->states))[i][3] == 2){
        std::string inputOutput;
        if( (*(fields->states))[i][0]==1){
            inputOutput="input";
        }
        if( (*(fields->states))[i][0]==2){
            inputOutput="output";
        }
        std::string Bus;
        if( (*(fields->states))[i][2]==2){
            Bus="";
        }
        if( (*(fields->states))[i][2]==1){
            Bus=(*(fields->allFields))[i].BusSizeVal;
            if((*(fields->states))[i][1]==1){
                Bus = "["+Bus+":0] ";
            }
            if((*(fields->states))[i][1]==2){
                Bus = "[0:" + Bus + "] ";
            }

        }
        std::string vartype;
        if((*(fields->states))[i][0]==2){vartype="wire ";}
        else{vartype="reg ";}
        std::string tempString = "." + std::string(std::string((*(fields->allFields))[i].name)) + "(" +std::string(std::string((*(fields->allFields))[i].name)) + ")" + "," +" ";
        std::string fieldString = vartype + Bus + std::string((*(fields->allFields))[i].name)  +";";
        portinputs+=tempString;
        ports+= fieldString +"\n";

        
        }
        if((*(fields->states))[i][3] == 1){
            std::string tempString = "."+std::string((*(fields->allFields))[i].name) + "(" + std::string((*(fields->allFields))[i].name) + ")" +"," +" ";
            std::string fieldString =  std::string("localparam ") + std::string((*(fields->allFields))[i].name ) +std::string(" = ") +std::string((*(fields->allFields))[i].BusSizeVal) +";"+ "\n";
            parameters+=fieldString;
            paraminputs+=tempString;
        }

        
    }
    std::size_t commaIndex = paraminputs.rfind(",");
    if(commaIndex != std::string::npos)
        paraminputs.replace(commaIndex,1,"");
    std::size_t commaIndex2 = portinputs.rfind(",");
    if(commaIndex2 != std::string::npos)
        portinputs.replace(commaIndex2,1,"");

    std::string outstring = ports + "\n" +"\n" + parameters;
    std::string DUT = moduleName +" "+ "#"+"(" + paraminputs +")"+ std::string("DUT") +"("+ portinputs + ")" +";";
    return outstring + "\n" +DUT;




}


std::string MakeStringModule(fieldManager* fields){
    std::string parameters = "";
    std::string ports = "";
    for(int i=0; i< fields->allFields->size();i++){
        if((*(fields->states))[i][3] == 2){
        std::string inputOutput;
        if( (*(fields->states))[i][0]==1){
            inputOutput="input";
        }
        if( (*(fields->states))[i][0]==2){
            inputOutput="output";
        }
        std::string Bus;
        if( (*(fields->states))[i][2]==2){
            Bus="";
        }
        if( (*(fields->states))[i][2]==1){
            Bus=(*(fields->allFields))[i].BusSizeVal;
            if((*(fields->states))[i][1]==1){
                Bus = "["+Bus+":0] ";
            }
            if((*(fields->states))[i][1]==2){
                Bus = "[0:" + Bus + "] ";
            }

        }
        std::string fieldString = inputOutput +" "+ Bus + std::string((*(fields->allFields))[i].name)  +",";
        ports+= fieldString +"\n";

        
        }
        if((*(fields->states))[i][3] == 1){
            std::string fieldString =  std::string("parameter ") + std::string((*(fields->allFields))[i].name ) +std::string(" = ") +std::string((*(fields->allFields))[i].BusSizeVal);
            parameters+=fieldString +std::string(", ");
        }

        
    }
    std::size_t commaIndex = parameters.rfind(",");
    if(commaIndex != std::string::npos)
        parameters.replace(commaIndex,1,"");
    std::size_t commaIndex2 = ports.rfind(",");
    if(commaIndex2 != std::string::npos)
        ports.replace(commaIndex2,1,"");
    std::string outstring;

    if(parameters.empty()){
     outstring =  std::string("(") + "\n" + ports + ")" +";";
    }
    else{
    outstring =  std::string("#(") + "\n"+ parameters +"\n" +  ")" +"\n"+"(" + "\n" + ports + ")" +";";

    }
    return outstring;
}
    



void generateTemplates(fieldManager* fields, char* argv[]){
    // button to print results
    Rectangle printResultsButton ={0,10,50,100};
    DrawRectangleRounded(printResultsButton,0.5,1, LIGHTGRAY);
    DrawText(TextFormat("Make File"),printResultsButton.x,printResultsButton.y+50, 10, DARKGRAY);

    if( CheckCollisionPointRec(GetMousePosition(), printResultsButton) ){
        DrawRectangleRoundedLines(printResultsButton,0.5,1,3,YELLOW);

    }
    else{
    DrawRectangleRoundedLines(printResultsButton,0.5,1,3,BLACK);

    }
    

    

    if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), printResultsButton) ){

        //write to file
        std::string filename = argv[1];
        std::string testbench("tb_" + filename);
        std::string testmodule(filename);
        std::ofstream printFile(testbench+".sv");
        std::ofstream printFilemodule(testmodule+".v");
        printFilemodule << "module " << filename <<std::endl
                        << MakeStringModule(fields) <<std::endl
                        <<std::endl
                        <<std::endl
                        << "endmodule" << std::endl;




        printFile << "`include \""<<testmodule << ".v\"" << std::endl
                  << "module "<<testbench <<"();" << std::endl
                  << std::endl
                  <<  MakeStringTestbench(fields,testmodule)//logic as to what that is
                  << std::endl
                  << std::endl
                  << std::endl
                  << "initial begin"<< std::endl
                  << "$dumpfile(" << "\""<< testbench <<".vcd" << "\""<< ");" <<std::endl 
                  << "$dumpvars(0,"<<testbench <<");" <<std::endl
                  << "#10" <<std::endl
                  << "$finish;" << std::endl
                  << std::endl
                  << std::endl
                  << std::endl
                  << std::endl
                  << "end" << std::endl
                  << std::endl
                  << std::endl
                  << std::endl
                  << "endmodule" << std::endl;

        printFile.close();
        /*
        printFile.open(testmodule, std::ios_base::out);
        if(!printFile.is_open()){
            exit(1);
        }
        else{

        }
        */
        exit(0);
    }


    //print results to file
    /*
    std::string filename = *argv;
    std::string testbench(filename);
    std::string testmodule(filename);
    std::fstream printFile;
    printFile.open(testmodule, std::ios_base::out);
    if(!printFile.is_open()){
        exit(1);
    }
    else{

    }
    */





    
}

void drawScreen(fieldManager* allTheFields,int framesCounter)
{
    std::vector<field> fields =  *(allTheFields->allFields);

    for(int i=0; i<fields.size();i++){

    Rectangle textBox = fields[i].textBox;
    Rectangle inputOutput =fields[i].inputOutput;
    Rectangle BigEndianLittle = fields[i].BigEndianLittle;
    Rectangle BusOrNot = fields[i].BusOrNot;
    Rectangle BusSize = fields[i].BusSize;
    Rectangle parameterOrNot = fields[i].parameterOrNot;

    int letterCount = fields[i].letterCount;
    int letterCountBus = fields[i].letterCountBusSize;




    Rectangle* rec[6] ={&textBox,&inputOutput,&BigEndianLittle,&BusSize,&BusOrNot,&parameterOrNot};

    for(int j=0;j<6;j++){
            DrawRectangleRounded(*rec[j],0.5,1, LIGHTGRAY);

            
				

            if(fields[i].higlightSection[1] )
            {
                if((*(allTheFields->states))[i][0] ==2){
                DrawText(TextFormat("Output"),rec[1]->x,rec[1]->y+25/2, 10, DARKGRAY);
                }
                if((*(allTheFields->states))[i][0] ==1){
                DrawText(TextFormat("Input"),rec[1]->x,rec[1]->y+25/2, 10, DARKGRAY);
                }
                if(((*(allTheFields->states))[i][0] ==0)){
                DrawText(TextFormat("Select"),rec[1]->x,rec[1]->y+12, 10, DARKGRAY);
                }
            }
            else{DrawRectangleRounded(*rec[1],0.5,1, LIGHTGRAY);} 

            if(fields[i].higlightSection[2]){
                if((*(allTheFields->states))[i][1] ==2){
                DrawText(TextFormat("Little Endian"),rec[2]->x,rec[2]->y+25/2, 10, DARKGRAY);
                }
                if((*(allTheFields->states))[i][1] ==1){
                DrawText(TextFormat("Big Endian"),rec[2]->x,rec[2]->y+25/2, 10, DARKGRAY);
                }
                if(((*(allTheFields->states))[i][1] ==0)){
                DrawText(TextFormat("Select"),rec[2]->x,rec[2]->y+12, 10, DARKGRAY);
                }
            }
            else{DrawRectangleRounded(*rec[2],0.5,1, LIGHTGRAY);}

            if(fields[i].higlightSection[4]){
                if((*(allTheFields->states))[i][2] ==2){
                DrawText(TextFormat("Not Bus"),rec[4]->x,rec[4]->y+25/2, 10, DARKGRAY);
                }
                if((*(allTheFields->states))[i][2] ==1){
                DrawText(TextFormat("Bus"),rec[4]->x,rec[4]->y+25/2, 10, DARKGRAY);
                }
                if(((*(allTheFields->states))[i][2] ==0)){
                DrawText(TextFormat("Select"),rec[4]->x,rec[4]->y+12, 10, DARKGRAY);
                }
            } 
            else{DrawRectangleRounded(*rec[4],0.5,1, LIGHTGRAY);}

            if(fields[i].higlightSection[5]){
                if((*(allTheFields->states))[i][3] ==2){
                DrawText(TextFormat("Port"),rec[5]->x,rec[5]->y+25/2, 10, DARKGRAY);
                }
                if((*(allTheFields->states))[i][3] ==1){
                DrawText(TextFormat("Parameter"),rec[5]->x,rec[5]->y+25/2, 10, DARKGRAY);
                }
                if(((*(allTheFields->states))[i][3] ==0)){
                DrawText(TextFormat("Select"),rec[5]->x,rec[5]->y+12, 10, DARKGRAY);
                }
            } 
            else{DrawRectangleRounded(*rec[5],0.5,1, LIGHTGRAY);}

          
            if (fields[i].mouseOnText[j]){
                DrawRectangleRoundedLines(*rec[j],0.5,1,3,YELLOW);
            } 
            else
            {
                DrawRectangleRoundedLines(*rec[j],0.5,1,3,DARKGRAY);
            }
    }

            DrawText(fields[i].name, (int)textBox.x + 5, (int)textBox.y + 8, 17, MAROON);
            DrawText(fields[i].BusSizeVal, (int)BusSize.x + 5, (int)BusSize.y + 8, 15, MAROON);


            DrawText(TextFormat("input chars: %i/%i", letterCount, MAX_INPUT_CHARS), textBox.x-110+15, textBox.y+10, 10, DARKGRAY);
            DrawText(TextFormat("input chars: %i/%i", letterCountBus, MAX_INPUT_CHARS_BUSSIZE), textBox.x-110+15, textBox.y+30, 10, DARKGRAY);


            if (fields[i].mouseOnText[0])
            {
                if (letterCount < MAX_INPUT_CHARS)
                {
                    // Draw blinking underscore char
                    if (((framesCounter/20)%2) == 0) DrawText("_", (int)textBox.x + 8 + MeasureText(fields[i].name, 17), (int)textBox.y + 12, 20, MAROON);
                }
                else DrawText("Press BACKSPACE to delete chars...", 230, 300, 20, GRAY);
            }
            if (fields[i].mouseOnText[3])
            {
                if (letterCount < MAX_INPUT_CHARS)
                {
                    // Draw blinking underscore char
                    if (((framesCounter/20)%2) == 0) DrawText("_", (int)BusSize.x + 8 + MeasureText(fields[i].BusSizeVal, 15), (int)BusSize.y + 12, 20, MAROON);
                }
                else DrawText("Press BACKSPACE to delete chars...", 230, 300, 20, GRAY);
            }





	
    }




    for(int i=fields.size()-1; i>=0;i--){
    //for(int i=0; i<fields.size();i++){

    Rectangle textBox = fields[i].textBox;
    Rectangle inputOutput =fields[i].inputOutput;
    Rectangle BigEndianLittle = fields[i].BigEndianLittle;
    Rectangle BusOrNot = fields[i].BusOrNot;
    Rectangle BusSize = fields[i].BusSize;
    Rectangle parameterOrNot = fields[i].parameterOrNot;

    int letterCount = fields[i].letterCount;
    int letterCountBus = fields[i].letterCountBusSize;




    Rectangle* rec[6] ={&textBox,&inputOutput,&BigEndianLittle,&BusSize,&BusOrNot,&parameterOrNot};
     for(int j=0;j<6;j++){
            if(fields[i].higlightSection[1]  && fields[i].textClicked[1]==1 && (*(allTheFields->states))[i][3]!=1)
            {
                
                Rectangle option1= {rec[1]->x,rec[1]->y+50,100,40};
                Rectangle option2= {rec[1]->x,rec[1]->y+90,100,40};
                DrawRectangleRounded(option1,0.5,1, DARKGREEN);
                DrawRectangleRounded(option2,0.5,1, DARKGREEN);
                DrawRectangleRoundedLines(option1,0.5,1,3, BLACK);
                DrawRectangleRoundedLines(option2,0.5,1,3, BLACK);

                
                if(!(CheckCollisionPointRec(GetMousePosition(), option1) == 0)){
                    //allTheFields->menuBarSelected=1;
                    DrawRectangleRoundedLines(option1,0.5,1,3, YELLOW);
                    if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && (CheckCollisionPointRec(GetMousePosition(), option1))|| (CheckCollisionPointRec(GetMousePosition(), option1) != 0)){
                        //allTheFields->menuBarSelected=0;
                        //allTheFields->menuClosed=1;
                        fields[i].higlightSection[1]=0;
                        fields[i].mouseOnText[1]=0;
                        fields[i].textClicked[1]=0;
                        (*(allTheFields->states))[i][0] =1;
                        //(*allTheFields->allFields)[i].stateSelected[0]=1;
                    }

                }
                if(!(CheckCollisionPointRec(GetMousePosition(), option2) == 0)){
                    DrawRectangleRoundedLines(option2,0.5,1,3, YELLOW);
                    if((IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) && (CheckCollisionPointRec(GetMousePosition(), option2))|| (CheckCollisionPointRec(GetMousePosition(), option2) != 0)){
                        fields[i].higlightSection[1]=0;
                        fields[i].mouseOnText[1]=0;
                        fields[i].textClicked[1]=0;
                        (*(allTheFields->states))[i][0] =2;
                        //(*allTheFields->allFields)[i].stateSelected[0]=2;
                    }
                }
                DrawText(TextFormat("Input"),rec[1]->x,rec[1]->y+50+25/2, 10, DARKGRAY);
                DrawText(TextFormat("Output"),rec[1]->x,rec[1]->y+90+25/2, 10, DARKGRAY);

                
            }
            else
            {
                if((*(allTheFields->states))[i][3]==1){
                DrawRectangleRounded(*rec[1],0.5,1, RED);
                }
                else{
                DrawRectangleRounded(*rec[1],0.5,1, LIGHTGRAY);

                }
                if((*(allTheFields->states))[i][0] ==2){
                DrawText(TextFormat("Output"),rec[1]->x,rec[1]->y+25/2, 10, DARKGRAY);
                }
                if((*(allTheFields->states))[i][0] ==1){
                DrawText(TextFormat("Input"),rec[1]->x,rec[1]->y+25/2, 10, DARKGRAY);
                }
                if(((*(allTheFields->states))[i][0] ==0)){
                DrawText(TextFormat("Select"),rec[1]->x,rec[1]->y+12, 10, DARKGRAY);
                }
            } 

            if(fields[i].higlightSection[2] && /*!fields[i].higlightSection[5] &&*/ fields[i].higlightSection[4] && fields[i].textClicked[2]==1&& (*(allTheFields->states))[i][3]!=1 && (*(allTheFields->states))[i][2] !=2){
            Rectangle option1= {rec[2]->x,rec[2]->y+50,100,40};
                Rectangle option2= {rec[2]->x,rec[2]->y+90,100,40};
                DrawRectangleRounded(option1,0.5,1, DARKGREEN);
                DrawRectangleRounded(option2,0.5,1, DARKGREEN);
                DrawRectangleRoundedLines(option1,0.5,1,3, BLACK);
                DrawRectangleRoundedLines(option2,0.5,1,3, BLACK);

                
                if(!(CheckCollisionPointRec(GetMousePosition(), option1) == 0)){
                    //allTheFields->menuBarSelected=1;
                    DrawRectangleRoundedLines(option1,0.5,1,3, YELLOW);
                    if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && (CheckCollisionPointRec(GetMousePosition(), option1))|| (CheckCollisionPointRec(GetMousePosition(), option1) != 0)){
                        //allTheFields->menuBarSelected=0;
                        //allTheFields->menuClosed=1;
                        fields[i].higlightSection[2]=0;
                        fields[i].mouseOnText[2]=0;
                        fields[i].textClicked[2]=0;
                        (*(allTheFields->states))[i][1] =1;
                        //(*allTheFields->allFields)[i]->stateSelected[1]=1;
                        
                    }

                }
                if(!(CheckCollisionPointRec(GetMousePosition(), option2) == 0)){
                    DrawRectangleRoundedLines(option2,0.5,1,3, YELLOW);
                    if((IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) && (CheckCollisionPointRec(GetMousePosition(), option2))|| (CheckCollisionPointRec(GetMousePosition(), option2) != 0)){
                        fields[i].higlightSection[2]=0;
                        fields[i].mouseOnText[2]=0;
                        fields[i].textClicked[2]=0;
                        (*(allTheFields->states))[i][1] =2;
                       // (*allTheFields->allFields)[i].stateSelected[1]=2;
                        
                    }
                }
                DrawText(TextFormat("Big Endian"),rec[2]->x,rec[2]->y+50+25/2, 10, DARKGRAY);
                DrawText(TextFormat("Little Endian"),rec[2]->x,rec[2]->y+90+25/2, 10, DARKGRAY);

                
            
            
            
            }
            else
            {
            if((*(allTheFields->states))[i][3]==1 || (*(allTheFields->states))[i][2] ==2){
                DrawRectangleRounded(*rec[2],0.5,1, RED);
                }
                else{
                DrawRectangleRounded(*rec[2],0.5,1, LIGHTGRAY);

                }
            if((*(allTheFields->states))[i][1] ==2){
                DrawText(TextFormat("Little Endian"),rec[2]->x,rec[2]->y+25/2, 10, DARKGRAY);
                }
                if((*(allTheFields->states))[i][1] ==1){
                DrawText(TextFormat("Big Endian"),rec[2]->x,rec[2]->y+25/2, 10, DARKGRAY);
                }
                if(((*(allTheFields->states))[i][1] ==0)){
                DrawText(TextFormat("Select"),rec[2]->x,rec[2]->y+12, 10, DARKGRAY);
                }
            }

            if(fields[i].higlightSection[4] && /*!fields[i].higlightSection[5] && */fields[i].textClicked[4]==1&& (*(allTheFields->states))[i][3]!=1){
                Rectangle option1= {rec[4]->x,rec[4]->y+50,100,40};
                Rectangle option2= {rec[4]->x,rec[4]->y+90,100,40};
                DrawRectangleRounded(option1,0.5,1, DARKGREEN);
                DrawRectangleRounded(option2,0.5,1, DARKGREEN);
                DrawRectangleRoundedLines(option1,0.5,1,3, BLACK);
                DrawRectangleRoundedLines(option2,0.5,1,3, BLACK);

                
                if(!(CheckCollisionPointRec(GetMousePosition(), option1) == 0)){
                    //allTheFields->menuBarSelected=1;
                    DrawRectangleRoundedLines(option1,0.5,1,3, YELLOW);
                    if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && (CheckCollisionPointRec(GetMousePosition(), option1)) || (CheckCollisionPointRec(GetMousePosition(), option1) != 0)){
                        //allTheFields->menuBarSelected=0;
                        //allTheFields->menuClosed=1;
                        fields[i].higlightSection[4]=0;
                        fields[i].mouseOnText[4]=0;
                        fields[i].textClicked[4]=0;
                        (*(allTheFields->states))[i][2] =1;
                        
                       // (*allTheFields->allFields)[i].stateSelected[2]=1;
                    }

                }
                if(!(CheckCollisionPointRec(GetMousePosition(), option2) == 0)){
                    DrawRectangleRoundedLines(option2,0.5,1,3, YELLOW);
                    if((IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) && (CheckCollisionPointRec(GetMousePosition(), option2))|| (CheckCollisionPointRec(GetMousePosition(), option2) != 0)){
                        fields[i].higlightSection[4]=0;
                        fields[i].mouseOnText[4]=0;
                        fields[i].textClicked[4]=0;
                        (*(allTheFields->states))[i][2] =2;
                       // (*allTheFields->allFields)[i].stateSelected[2]=2;

                        
                    }
                }
                DrawText(TextFormat("Bus"),rec[4]->x,rec[4]->y+50+25/2, 10, DARKGRAY);
                DrawText(TextFormat("Not Bus"),rec[4]->x,rec[4]->y+90+25/2, 10, DARKGRAY);
                
                
                
            }
            else
            {
                if((*(allTheFields->states))[i][3]==1){
                DrawRectangleRounded(*rec[4],0.5,1, RED);
                }
                else{
                DrawRectangleRounded(*rec[4],0.5,1, LIGHTGRAY);

                }
                
                if((*(allTheFields->states))[i][2] ==2){
                DrawText(TextFormat("Not Bus"),rec[4]->x,rec[4]->y+25/2, 10, DARKGRAY);
                }
                if((*(allTheFields->states))[i][2] ==1){
                DrawText(TextFormat("Bus"),rec[4]->x,rec[4]->y+25/2, 10, DARKGRAY);
                }
                if(((*(allTheFields->states))[i][2] ==0)){
                DrawText(TextFormat("Select"),rec[4]->x,rec[4]->y+12, 10, DARKGRAY);
                }


            }







            if(fields[i].higlightSection[5] &&  fields[i].textClicked[5]==1){
                Rectangle option1= {rec[5]->x,rec[5]->y+50,100,40};
                Rectangle option2= {rec[5]->x,rec[5]->y+90,100,40};
                DrawRectangleRounded(option1,0.5,1, DARKGREEN);
                DrawRectangleRounded(option2,0.5,1, DARKGREEN);
                DrawRectangleRoundedLines(option1,0.5,1,3, BLACK);
                DrawRectangleRoundedLines(option2,0.5,1,3, BLACK);

                
                if((CheckCollisionPointRec(GetMousePosition(), option1) != 0)){
                    //allTheFields->menuBarSelected=1;
                    DrawRectangleRoundedLines(option1,0.5,1,3, YELLOW);
                    if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && (CheckCollisionPointRec(GetMousePosition(), option1)) || (CheckCollisionPointRec(GetMousePosition(), option1) != 0)){
                     
                        fields[i].higlightSection[5]=0;
                        fields[i].mouseOnText[5]=0;
                        fields[i].textClicked[5]=0;
                         (*(allTheFields->states))[i][3] =1;
                        
                        
                    }
                    

                }
                if((CheckCollisionPointRec(GetMousePosition(), option2) != 0)){
                    DrawRectangleRoundedLines(option2,0.5,1,3, YELLOW);
                    if((IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) && (CheckCollisionPointRec(GetMousePosition(), option2)) || (CheckCollisionPointRec(GetMousePosition(), option2) != 0)){
                       fields[i].higlightSection[5]=0;
                        fields[i].mouseOnText[5]=0;
                        fields[i].textClicked[5]=0;
                        (*(allTheFields->states))[i][3] =2;
                        

                        
                    }
                }
                DrawText(TextFormat("Parameter"),rec[5]->x,rec[5]->y+50+25/2, 10, DARKGRAY);
                DrawText(TextFormat("Port"),rec[5]->x,rec[5]->y+90+25/2, 10, DARKGRAY);   
            }
            else
            {
                DrawRectangleRounded(*rec[5],0.5,1, LIGHTGRAY);
                if((*(allTheFields->states))[i][3] ==2){
                DrawText(TextFormat("Port"),rec[5]->x,rec[5]->y+25/2, 10, DARKGRAY);
                }
                if((*(allTheFields->states))[i][3] ==1){
                DrawText(TextFormat("Parameter"),rec[5]->x,rec[5]->y+25/2, 10, DARKGRAY);
                }
                if(((*(allTheFields->states))[i][3] ==0)){
                DrawText(TextFormat("Select"),rec[5]->x,rec[5]->y+12, 10, DARKGRAY);
                }

                //fields[i].higlightSection[5] = 0;
            }

          
            
         
    }
    }

	
	
}

void addField(fieldManager* fields){
    Rectangle addFieldRect = {0,325,50,100};
    DrawRectangleRounded(addFieldRect,0.5,1,LIGHTGRAY);
    if(CheckCollisionPointRec(GetMousePosition(), addFieldRect)){
    DrawRectangleRoundedLines(addFieldRect,0.5,1,3,RED);
    }
    else{
        DrawRectangleRoundedLines(addFieldRect,0.5,1,3,DARKGRAY);
    }
    DrawText(TextFormat("ADD field"), 0, 325+50, 10, DARKGRAY);

    if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), addFieldRect) && fields->menuBarSelected==0){
        
        int widthOfTextField=225;
        int widthofNumbeField=140;
        int widthofBoolField=60;
        int seperationWidth=5;
        int horizontalShift=140;
        int fieldheight=fields->bottomHeight;

            Rectangle BusSize= { screenWidth/2.0f+widthOfTextField/2+seperationWidth*5 +widthofBoolField*4-horizontalShift, fieldheight, widthofNumbeField, 50 };
           Rectangle BusOrNot= { screenWidth/2.0f+widthOfTextField/2+seperationWidth*3 +widthofBoolField*2-horizontalShift, fieldheight, widthofBoolField, 50 };
    Rectangle BigEndianLittle= { screenWidth/2.0f+widthOfTextField/2+seperationWidth*4 +widthofBoolField*3-horizontalShift, fieldheight, widthofBoolField, 50 };
       Rectangle inputOutput = { screenWidth/2.0f+widthOfTextField/2+seperationWidth*2 +widthofBoolField -horizontalShift, fieldheight, widthofBoolField, 50 };
    Rectangle parameterOrNot = {screenWidth/2.0f+widthOfTextField/2+seperationWidth-horizontalShift, fieldheight, widthofBoolField, 50 };
           Rectangle textBox = { screenWidth/2.0f-widthOfTextField/2-horizontalShift, fieldheight, widthOfTextField, 50 };

        field tempField{"\0","\0",0,0,BusOrNot,BusSize,BigEndianLittle,inputOutput,textBox,parameterOrNot};
        //fields->topHeight-=55;
        fields->bottomHeight+=55;
        std::vector<int> temp = std::vector<int>(4);
        fields->states->push_back(temp);
        

        fields->allFields->push_back(tempField);
        //fields.topHeight+=55;


    }


}






/*
void detectCollision(Rectangle rec){

        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), entryboxes[i].textBox))
        {
            entryboxes[i].textClicked = true;
            entryboxes[i].mouseOnText = true;
        }
        else if (CheckCollisionPointRec(GetMousePosition(), entryboxes[i].textBox) ){
            entryboxes[i].mouseOnText = true;
        } 
      
        else{
            if(!entryboxes[i].textClicked){
            entryboxes[i].mouseOnText = false;
            }
        }

        if((IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && (CheckCollisionPointRec(GetMousePosition(), entryboxes[i].textBox) == 0) ))
        {
            entryboxes[i].textClicked = false;
        }



}
*/




void drawCursorBar(fieldManager* fields, cursorBar* cursor){
int CursorStart= 20;
int CursorEnd=screenHeight-CursorStart;
addField(fields);

 Rectangle scrollBackGround = {765,CursorStart,35,CursorEnd-CursorStart};
        //float heightposition; //dependent on user input
        cursor->length =  (CursorEnd-CursorStart)/(fields->bottomHeight-fields->topHeight) * (CursorEnd-CursorStart);
        Rectangle userScroll = {765,cursor->heightposition,35,cursor->length};
        //collision with scroll field
        if((IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsMouseButtonDown(MOUSE_LEFT_BUTTON)) && (CheckCollisionPointRec(GetMousePosition(), userScroll) || CheckCollisionPointRec(GetMousePosition(), scrollBackGround)) ){
            
            cursor->heightposition=GetMouseY();

            Rectangle userScroll = {780,cursor->heightposition,20,cursor->length};
            fields->currentPos=cursor->heightposition;
            if(cursor->heightposition > CursorEnd-cursor->length){

                fields->currentPos=CursorEnd-cursor->length;
            }
       
            int tempTranslation=fields->bottomHeight-fields->currentPos*((fields->bottomHeight-fields->topHeight)/(CursorEnd-CursorStart));
            for(int i=(*fields->allFields).size()-1; i>=0;i--){
                

                (*fields->allFields)[i].textBox.y= tempTranslation;
                (*fields->allFields)[i].inputOutput.y= tempTranslation;
                (*fields->allFields)[i].BigEndianLittle.y=tempTranslation;
                (*fields->allFields)[i].BusSize.y= tempTranslation;
                (*fields->allFields)[i].BusOrNot.y= tempTranslation;
                (*fields->allFields)[i].parameterOrNot.y= tempTranslation;
                tempTranslation-=55;
             
            }
            


        }
        if(cursor->heightposition > screenHeight-cursor->length)
        {
        userScroll = {765,CursorEnd-cursor->length,35,cursor->length};
        }
        
        if(fields->bottomHeight > screenHeight){
        DrawRectangleRounded(scrollBackGround,0.5,1,LIGHTGRAY);
        DrawRectangleRounded(userScroll,0.5,1,DARKGRAY);
        }
        


}


//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    // Initialization
    //--------------------------------------------------------------------------------------
 	std::vector<field> entryboxes;
    std::vector<std::vector<int>> stateofButtons;
    
    fieldManager fields= {&entryboxes,0,0};
    fields.states=&stateofButtons;

    cursorBar cursor = {20,0};
    float fieldheight=20;
    fields.topHeight=20;
    fields.bottomHeight+=fields.topHeight;

    int widthOfTextField=225;
    int widthofNumbeField=140;
    int widthofBoolField=60;
    int seperationWidth=5;
    int horizontalShift=140;
    for(int i=0; i<1;i++){
        int letterCount = 0;
            Rectangle BusSize= { screenWidth/2.0f+widthOfTextField/2+seperationWidth*5 +widthofBoolField*4-horizontalShift, fieldheight, widthofNumbeField, 50 };
           Rectangle BusOrNot= { screenWidth/2.0f+widthOfTextField/2+seperationWidth*3 +widthofBoolField*2-horizontalShift, fieldheight, widthofBoolField, 50 };
    Rectangle BigEndianLittle= { screenWidth/2.0f+widthOfTextField/2+seperationWidth*4 +widthofBoolField*3-horizontalShift, fieldheight, widthofBoolField, 50 };
       Rectangle inputOutput = { screenWidth/2.0f+widthOfTextField/2+seperationWidth*2 + widthofBoolField -horizontalShift, fieldheight, widthofBoolField, 50 };
           Rectangle textBox = { screenWidth/2.0f-widthOfTextField/2-horizontalShift, fieldheight, widthOfTextField, 50 };
           Rectangle parameterOrNot = {screenWidth/2.0f+widthOfTextField/2+seperationWidth-horizontalShift, fieldheight, widthofBoolField, 50 };
        field firstentrybox{"\0","\0",letterCount,0,BusOrNot,BusSize,BigEndianLittle,inputOutput,textBox,parameterOrNot};
        
        
        std::vector<int> temp = std::vector<int>(4);
        stateofButtons.push_back(temp);
        //stateofButtons[i].push_back(0);
        //stateofButtons[i].push_back(0);
        //stateofButtons[i].push_back(0);
        //tateofButtons[i].push_back(0);
        

        entryboxes.push_back(firstentrybox);
        fieldheight+=55;
        fields.bottomHeight=fieldheight;
    }


    InitWindow(screenWidth, screenHeight, "raylib [text] example - input box");

  
    bool mouseOnText = false;

    int framesCounterParamName = 0;
    int framesCounterBusSize = 0;

    SetTargetFPS(30);               // Set our game to run at 10 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        for(int i=0; i<entryboxes.size();i++){
        Rectangle* rec[6] ={&entryboxes[i].textBox,&entryboxes[i].inputOutput,&entryboxes[i].BigEndianLittle,&entryboxes[i].BusSize,&entryboxes[i].BusOrNot,&entryboxes[i].parameterOrNot};
        for(int j=0; j<6 ;j++){
            
            if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), *rec[j]))
            {
                //if(fields.menuBarSelected==0){
                entryboxes[i].textClicked[j] = true;
                entryboxes[i].mouseOnText[j] = true;
                entryboxes[i].higlightSection[j] = !entryboxes[i].higlightSection[j];
                if(j==1){
                    entryboxes[i].higlightSection[j]=1;
                }
                if(j==2){
                    entryboxes[i].higlightSection[j]=1;
                }
                if(j==4){
                    entryboxes[i].higlightSection[j]=1;
                }
                if(j==5){
                    entryboxes[i].higlightSection[j]=1;
                }

                //}
                
            }
            else if (CheckCollisionPointRec(GetMousePosition(), *rec[j]) ){
                if(fields.menuBarSelected==0){
                entryboxes[i].mouseOnText[j] = true;
                //fields.menuClosed=1;

                }
            } 
        
            else{
                if(!entryboxes[i].textClicked[j]){
                entryboxes[i].mouseOnText[j] = false;


                }
            }

            if((IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && (CheckCollisionPointRec(GetMousePosition(), *rec[j]) == 0) ))
            {
                entryboxes[i].textClicked[j] = false;
            }
        }

        //making sure 1 is true


        

        //alter the string array for the parameter name
        if (entryboxes[i].mouseOnText[0])
        {
            // Set the window's cursor to the I-Beam
            SetMouseCursor(MOUSE_CURSOR_IBEAM);

            // Get char pressed (unicode character) on the queue
            int key = GetCharPressed();

            // Check if more characters have been pressed on the same frame
            while (key > 0)
            {
                // NOTE: Only allow keys in range [32..125]
                if ((key >= 32) && (key <= 125) && (entryboxes[i].letterCount < MAX_INPUT_CHARS))
                {
                    entryboxes[i].name[entryboxes[i].letterCount] = (char)key;
                    entryboxes[i].name[entryboxes[i].letterCount+1] = '\0'; // Add null terminator at the end of the string.
                    entryboxes[i].letterCount++;
                }

                if(MAX_INPUT_CHARS < entryboxes[i].letterCount){

                    free(entryboxes[i].name);
                    for(int j=0;j<(sizeof(entryboxes[j].name)/sizeof(char));j++){
                    entryboxes[i].name[j]={'\0'};
                }
                    entryboxes[i].letterCount=0;


                }

                key = GetCharPressed();  // Check next character in the queue
            }

            if (IsKeyPressed(KEY_BACKSPACE))
            {
                entryboxes[i].letterCount--;
                if (entryboxes[i].letterCount < 0) entryboxes[i].letterCount = 0;
                entryboxes[i].name[entryboxes[i].letterCount] = '\0';
            }
				if (IsKeyPressedRepeat(KEY_BACKSPACE))
            {
                entryboxes[i].letterCount--;
                if (entryboxes[i].letterCount < 0) entryboxes[i].letterCount = 0;
                entryboxes[i].name[entryboxes[i].letterCount] = '\0';
            }
				if (IsKeyPressed(KEY_ENTER)) //enter new param
            {
                entryboxes[i].letterCount--;
                if (entryboxes[i].letterCount < 0) entryboxes[i].letterCount = 0;
                entryboxes[i].name[entryboxes[i].letterCount] = '\0';
            }
				if (IsKeyPressed(KEY_TAB)) // enter new 
            {
                entryboxes[i].letterCount--;
                if (entryboxes[i].letterCount < 0) entryboxes[i].letterCount = 0;
                entryboxes[i].name[entryboxes[i].letterCount] = '\0';
            }
        }

        // alter String array for the Bus Size
        if (entryboxes[i].mouseOnText[3])
        {
            // Set the window's cursor to the I-Beam
            SetMouseCursor(MOUSE_CURSOR_IBEAM);

            // Get char pressed (unicode character) on the queue
            int key = GetCharPressed();

            // Check if more characters have been pressed on the same frame
            while (key > 0)
            {
                // NOTE: Only allow keys in range [32..125]
                if ((key >= 32) && (key <= 125) && (entryboxes[i].letterCountBusSize < MAX_INPUT_CHARS_BUSSIZE))
                {
                    entryboxes[i].BusSizeVal[entryboxes[i].letterCountBusSize] = (char)key;
                    entryboxes[i].BusSizeVal[entryboxes[i].letterCountBusSize+1] = '\0'; // Add null terminator at the end of the string.
                    entryboxes[i].letterCountBusSize++;
                }

                if(MAX_INPUT_CHARS_BUSSIZE < entryboxes[i].letterCountBusSize){

                    free(entryboxes[i].name);
                    for(int j=0;j<(sizeof(entryboxes[j].BusSizeVal)/sizeof(char));j++){
                    entryboxes[i].BusSizeVal[j]={'\0'};
                }
                    entryboxes[i].letterCountBusSize=0;


                }

                key = GetCharPressed();  // Check next character in the queue
            }

            if (IsKeyPressed(KEY_BACKSPACE))
            {
                entryboxes[i].letterCountBusSize--;
                if (entryboxes[i].letterCountBusSize < 0) entryboxes[i].letterCountBusSize = 0;
                entryboxes[i].BusSizeVal[entryboxes[i].letterCountBusSize] = '\0';
            }
				if (IsKeyPressedRepeat(KEY_BACKSPACE))
            {
                entryboxes[i].letterCountBusSize--;
                if (entryboxes[i].letterCountBusSize < 0) entryboxes[i].letterCountBusSize = 0;
                entryboxes[i].BusSizeVal[entryboxes[i].letterCountBusSize] = '\0';
            }
				if (IsKeyPressed(KEY_ENTER)) //enter new param
            {
                entryboxes[i].letterCountBusSize--;
                if (entryboxes[i].letterCountBusSize < 0) entryboxes[i].letterCountBusSize = 0;
                entryboxes[i].BusSizeVal[entryboxes[i].letterCountBusSize] = '\0';
            }
				if (IsKeyPressed(KEY_TAB)) // enter new 
            {
               SetMousePosition(GetMouseX(),GetMouseY()+40);
            }
        }
        //else SetMouseCursor(MOUSE_CURSOR_DEFAULT);

        if (entryboxes[i].mouseOnText[0] || entryboxes[i].mouseOnText[3]) framesCounterParamName++;
        else framesCounterParamName = 0;






        }
        

       /*
    Rectangle scrollBackGround = {765,0,35,450};
        float heightposition; //dependent on user input
        float length = screenHeight/(fields.bottomHeight-fields.topHeight) * screenHeight;
        Rectangle userScroll = {765,heightposition,35,length};
        //collision with scroll field
        if((IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsMouseButtonDown(MOUSE_LEFT_BUTTON)) && (CheckCollisionPointRec(GetMousePosition(), userScroll) || CheckCollisionPointRec(GetMousePosition(), scrollBackGround)) ){
            
            heightposition=GetMouseY();
            Rectangle userScroll = {780,heightposition,20,length};
            if(!checkIfCursorInSection(GetCollisionRec(scrollBackGround,userScroll),userScroll))
            {
            userScroll = {765,450-length,35,length};
            }
            for(int i=0; i<(fields.allFields)->size();i++){
                //dynamicaly allocate new rectangles to not permanetly alter the original ones
                    
                (*fields.allFields)[i].textBox.y= (*fields.allFields)[i].textBox.y -userScroll.y;
                (*fields.allFields)[i].inputOutput.y= (*fields.allFields)[i].inputOutput.y-userScroll.y;
                (*fields.allFields)[i].BigEndianLittle.y= (*fields.allFields)[i].BigEndianLittle.y-userScroll.y;
                (*fields.allFields)[i].BusSize.y= (*fields.allFields)[i].BusSize.y-userScroll.y;
                (*fields.allFields)[i].BusOrNot.y= (*fields.allFields)[i].BusOrNot.y-userScroll.y;
                
             
            }

        }
        if(!checkIfCursorInSection(GetCollisionRec(scrollBackGround,userScroll),userScroll))
        {
        userScroll = {765,450-length,35,length};
        }
        */
        

        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        

            ClearBackground(RAYWHITE);
            DrawText(TextFormat("parameter/port name"), screenWidth/2.0f-widthOfTextField/2-horizontalShift, 5, 10, DARKGRAY);
            DrawText(TextFormat("port?"), screenWidth/2.0f+widthOfTextField/2+seperationWidth-horizontalShift, 5, 2, DARKGRAY);
            DrawText(TextFormat("in/out"), screenWidth/2.0f+widthOfTextField/2+seperationWidth*2 + widthofBoolField -horizontalShift, 5, 2, DARKGRAY);
            DrawText(TextFormat("endian"), screenWidth/2.0f+widthOfTextField/2+seperationWidth*4 +widthofBoolField*3-horizontalShift+5, 5, 2, DARKGRAY);
            DrawText(TextFormat("Bus?"), screenWidth/2.0f+widthOfTextField/2+seperationWidth*3 +widthofBoolField*2-horizontalShift, 5, 2, DARKGRAY);
            DrawText(TextFormat("Value/vector dimension"), screenWidth/2.0f+widthOfTextField/2+seperationWidth*5 +widthofBoolField*4-horizontalShift+5, 5, 2, DARKGRAY);
/*
            DrawText(TextFormat("Port"), 5, 50, 12, RED);DrawText(TextFormat("Parameter"), 65, 50, 12, DARKGREEN);
            DrawText(TextFormat("input"), 5, 65, 12, RED);DrawText(TextFormat("Output"), 65, 65, 12, DARKGREEN);
            DrawText(TextFormat("Bus"), 5, 80, 12, RED);DrawText(TextFormat("Not Bus"), 65, 80, 12, DARKGREEN);
            DrawText(TextFormat("big Endian"), 5, 95, 12, RED);DrawText(TextFormat("little Endian"), 65, 95, 12, DARKGREEN);
*/
            drawCursorBar(&fields,&cursor);
            drawScreen(&fields,framesCounterParamName);
            generateTemplates(&fields,argv);
           
            //DrawRectangleRounded(scrollBackGround,0.5,1,LIGHTGRAY);
            //DrawRectangleRounded(userScroll,0.5,1,DARKGRAY);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    entryboxes = std::vector<field>();
    stateofButtons = std::vector<std::vector<int>>();
    return 0;

}





// Check if any key is pressed
// NOTE: We limit keys check to keys between 32 (KEY_SPACE) and 126
bool IsAnyKeyPressed()
{
    bool keyPressed = false;
    int key = GetKeyPressed();

    if ((key >= 32) && (key <= 126)) keyPressed = true;

    return keyPressed;
}
