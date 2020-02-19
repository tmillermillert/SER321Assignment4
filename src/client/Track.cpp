#include "Track.hpp"
#include <iostream>
#include <stdlib.h>

/**
 * Copyright 2020 Marcus Miller,
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * 
 * Purpose: Track is a class whose properties describe a single
 * media work -- song or video/clip.
 * Ser321 Principles of Distributed Software Systems
 * see http://pooh.poly.asu.edu/Ser321
 * @author Marcus Miller mtmille5@asu.edu
 * @version February 2020
 */
Track::Track(){
   track = "";
   artist = "";
   album = "";
   duration = 0;
   rank = "";
   fileName = "";
}

Track::Track(string aTrack, string anArtist,
      string anAlbum, int aDuration, string aRank, string aFileName) {
   track = aTrack;
   artist = anArtist;
   album = anAlbum;
   duration = aDuration;
   rank = aRank;
   fileName = aFileName;
}

Track::Track(const Json::Value& jsonObj){
   Json::Value::Members mbr = jsonObj.getMemberNames();
   for(vector<string>::const_iterator i = mbr.begin(); i!= mbr.end(); i++){
      //cout << *i << " " << endl;
      Json::Value jsonM = jsonObj[*i];
      if(*i=="name"){//for fm
         track = jsonM.asString();
      }else if(*i=="artist"){//for fm
         artist = jsonM["name"].asString();
      }else if(*i=="duration"){//for fm
         duration = std::stoi(jsonM.asString());
      }else if(*i=="@attr"){//for fm
         rank = jsonM["rank"].asString();
      }else if(*i=="trackMTM"){//for album.json
         track = jsonM.asString();
      }else if(*i=="artistMTM"){//for album.json
         artist = jsonM.asString();
      }else if(*i=="albumMTM"){//for album.json
         album = jsonM.asString();
      }else if(*i=="durationMTM"){//for album.json
         duration = jsonM.asInt();
      }else if(*i=="rankMTM"){
         rank = jsonM.asString();
      }else if(*i=="fileNameMTM"){
         fileName = jsonM.asString();
      }
   }
}

Track::Track(string jsonString){
   Json::Reader reader;
   Json::Value root;
   bool parseSuccess = reader.parse(jsonString,root,false);
   if(parseSuccess){
      cout << "successful parse" << endl;
      Json::Value::Members mbr = root.getMemberNames();
      for(vector<string>::const_iterator i = mbr.begin(); i!= mbr.end(); i++){
         //cout << *i << " " << endl;
         Json::Value jsonM = root[*i];
         if(*i=="name"){//for fm
            track = jsonM.asString();
         }else if(*i=="artist"){//for fm
            artist = jsonM["name"].asString();
         }else if(*i=="duration"){//for fm
            duration = std::stoi(jsonM.asString());
         }else if(*i=="@attr"){//for fm
            rank = jsonM["rank"].asString();
         }else if(*i=="trackMTM"){//for album.json
            track = jsonM.asString();
         }else if(*i=="artistMTM"){//for album.json
            artist = jsonM.asString();
         }else if(*i=="albumMTM"){//for album.json
            album = jsonM.asString();
         }else if(*i=="durationMTM"){//for album.json
            duration = jsonM.asInt();
         }else if(*i=="rankMTM"){
            rank = jsonM.asString();
         }else if(*i=="fileNameMTM"){
            fileName = jsonM.asString();
         }
      }
   }else{
      cout << "Track constructor parse error with input: " << jsonString
         << endl;
   }
}

Track::~Track() {
   //cout << "Track destructor.\n";
   track = "";
   artist = "";
   album = "";
   rank = "";
   duration = 0;
   fileName = "";
}

string Track::toJsonString(){
   string ret = "{}";
   Json::Value jsonLib;
   jsonLib["trackMTM"] = track;
   jsonLib["artistMTM"] = artist;
   jsonLib["albumMTM"] = album;
   jsonLib["rankMTM"] = rank;
   jsonLib["fileNameMTM"] = fileName;
   jsonLib["durationMTM"] = duration;
   ret = jsonLib.toStyledString();
   return ret;
}

Json::Value Track::toJson(){
   Json::Value jsonLib;
   jsonLib["trackMTM"] = track;
   jsonLib["artistMTM"] = artist;
   jsonLib["albumMTM"] = album;
   jsonLib["rankMTM"] = rank;
   jsonLib["fileNameMTM"] = fileName;
   jsonLib["durationMTM"] = duration;
   return jsonLib;
}

void Track::setValues(string aTrack, string anArtist,
      string anAlbum, int aDuration, string aRank, string aFileName) {
   track = aTrack;
   artist = anArtist;
   album = anAlbum;
   duration = aDuration;
   rank = aRank;
   fileName = aFileName;
}

void Track::print(){
   cout << "Track " << track << " artist " << artist << " album " << album 
      << " duration " << duration << " rank " << rank << " fileName " 
      << fileName << "\n";
}

string Track::getTrackTime(){
   int tmp = duration;
   int hours = tmp / 3600;
   tmp %= 3600;
   int minutes = tmp / 60;
   tmp %= 60;
   int seconds = tmp;
   char buffer[9];
   sprintf(buffer, "%02d:%02d:%02d", hours, minutes, seconds);
   string trackTime = buffer;
   return trackTime;
}
