#include "MediaClientGui.cpp"
#include "MusicLibrary.hpp"

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Multiline_Input.H>
#include <FL/Fl_Check_Browser.H>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>
#include <stdlib.h>
#include <sstream>
#include <thread>
#include <json/json.h>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Exception.hpp>
#include <algorithm>
using namespace std;

std::string cmd;
void run(){
   system(cmd.c_str());
}

/**
 * Copyright (c) 2020 Tim Lindquist,
 * Software Engineering,
 * Arizona State University at the Polytechnic campus
 * <p/>
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation version 2
 * of the License.
 * <p/>
 * This program is distributed in the hope that it will be useful,
 * but without any warranty or fitness for a particular purpose.
 * <p/>
 * Please review the GNU General Public License at:
 * http://www.gnu.org/licenses/gpl-2.0.html
 * see also: https://www.gnu.org/licenses/gpl-faq.html
 * so you are aware of the terms and your rights with regard to this software.
 * Or, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,USA
 * <p/>
 * Purpose: C++ FLTK client UI for Music management.
 * This class extends the Gui component class MediaClientGui and demonstrates
 * sample control functions that respond to button clicks and menu item selects.
 * This software is meant to run on Linux and MacOS using g++.
 * Students may use this source code as the basis for their solution to
 * the programming projects in Ser321 to create a distributed music browser.
 * <p/>
 * This example depends on the library curlpp to download a file or make
 * a URL request. This should be used in problem solutions to search
 * last.fm for album and track information. Download the library from: 
 *   http://www.curlpp.org/
 * Extract the archive and from a terminal change to the directory created.
 * Execute the following commands looking for errors. Note the following steps
 * require that libcurl first be installed, if its not already present.
 *   cmake ./
 *   make
 *   sudo make install
 *   sudo ldconfig
 * To see the make options available do: make help
 * you may want to note the files that are installed by make install since there
 * does not appear to be any "uninstall" target in the makefile.
 *
 * To use this library, the compile and link must reference the appropriate
 * libraries:
 * g++ -std=c++14 MusicBrowser.cpp -o exampleCurlPP -lstdc++ -ljsoncpp -lcurlpp -lcurl
 * If the includes and libraries are not installed in a location known to g++, then
 * the command will need -I and -L options which point g++ to the include and
 * library files respectively.
 *
 * <p/>
 * Ser321 Principles of Distributed Software Systems
 * see http://pooh.poly.asu.edu/Ser321
 * @author Tim Lindquist (Tim.Lindquist@asu.edu) CIDSE - Software Engineering,
 *                       IAFSE, ASU at the Polytechnic campus
 * @author Marcus Miller (mtmille5@asu.edu)
 * @file    MusicBrowser.cpp
 * @date    Febuary, 2020
 **/
class MusicBrowser : public MediaClientGui {

   public:

      std::string userId;
      std::string lastfmkey;

      std::thread * playThread;
      MusicLibrary * library;
      Album searchedAlbum;

      /** ClickedX is one of the callbacks for GUI controls.
       * Callbacks need to be static functions. But, static functions
       * cannot directly access instance data. This program uses "userdata"
       * to get around that by passing the instance to the callback
       * function. The callback then accesses whatever GUI control object
       * that it needs for implementing its functionality.
       */
      static void ClickedX(Fl_Widget * w, void * userdata) {
         std::cout << "You clicked Exit" << std::endl;
         exit(1);
      }

      /**
       * Static search button callback method. 
       */
      static void SearchCallbackS(Fl_Widget*w, void*data) {
         MusicBrowser *o = (MusicBrowser*)data;
         cout << "Search Clicked. You asked for a last.fm search of album: " <<
            o->albSrchInput->value() << " by artist: " <<
            o->artSrchInput->value() << endl;
         string artistSearched(o->artSrchInput->value());
         std::replace(artistSearched.begin(), artistSearched.end(), ' ', '+'); 
         string albumSearched(o->albSrchInput->value());
         std::replace(albumSearched.begin(), albumSearched.end(), ' ', '+'); 
         try{
            std::string url = "http://ws.audioscrobbler.com/2.0/?method=album.getinfo&artist=" + artistSearched + "&album=" + albumSearched + "&format=json&api_key=";
            url = url + o->lastfmkey;
            cout << "sending request url: " << url << endl;
            std::ostringstream os;
            curlpp::Easy myRequest;
            myRequest.setOpt(new curlpp::options::WriteStream(&os));
            //curlpp::options::Url myUrl(std::string(url));
            myRequest.setOpt(new curlpp::options::Url(url.c_str()));
            myRequest.perform();
            std::string aString = os.str();
            std::cout << aString << std::endl;
            Json::Reader reader;
            Json::Value jsonObj;
            bool parseSuccess = reader.parse(aString, jsonObj, false);
            if(parseSuccess){
               jsonObj = jsonObj["album"];
            }
            else{
               cout << "Album constructor parse error with input: " <<
                     aString << endl;
               return;
            }
            o->searchedAlbum = Album(jsonObj);
            map<string, Track> newTracks;
            for (auto track : o->searchedAlbum.tracks){
               track.second.album = o->searchedAlbum.name;
               track.second.fileName = track.second.track + ".mp3";
               newTracks.insert(pair<string, Track>(track.first, track.second));
            }
            o->searchedAlbum.tracks = newTracks;
            o->buildTree();

         }catch ( curlpp::LogicError & e ) {
            std::cout << e.what() << std::endl;
         }
         catch ( curlpp::RuntimeError & e ) {
            std::cout << e.what() << std::endl;
         }
      }

      // Static menu callback method
      static void TreeCallbackS(Fl_Widget*w, void*data) {
         MusicBrowser* o = (MusicBrowser*)data;
         o->TreeCallback(); //call the instance callback method
      }

      /**
       * TreeCallback is a callback for tree selections, deselections, expand or
       * collapse.
       */
      void TreeCallback() {
         // Find item that was clicked
         Fl_Tree_Item *item = (Fl_Tree_Item*)tree->item_clicked();
         cout << "Tree callback. Item selected: ";
         if ( item ) {
            cout << item->label();
         } else {
            cout << "none";
         }
         cout << endl;
         std::string aStr("unknown");
         std::string aTitle(item->label());
         switch ( tree->callback_reason() ) {  // reason callback was invoked
            case       FL_TREE_REASON_NONE: {aStr = "none"; break;}
            case     FL_TREE_REASON_OPENED: {aStr = "opened";break;}
            case     FL_TREE_REASON_CLOSED: {aStr = "closed"; break;}
            case   FL_TREE_REASON_SELECTED: {
               if(item->depth() == 2){
                  aStr = "Clicked Album";
                  Album album;
                  if(library){
                     cout << "trying to get: " << item->label() << endl;
                     album = library->get(aTitle);
                     if(album.name == ""){
                        album = searchedAlbum;
                     }
                  }else{
                     cout << "library entry not found" << endl;
                     break;
                  }
                  //album.print();
                  trackInput->value("");
                  albumInput->value(album.name.c_str());
                  authorInput->value(album.artist.c_str());
                  rankInput->value("");
                  fileNameInput->value("");
                  genreChoice->clear();
                  vector<string>::iterator it = album.genres.begin();
                  while(it != album.genres.end()){
                     genreChoice->add(it->c_str());
                     it++;
                  }
                  genreChoice->value(0);
                  timeInput->value(album.runTime.c_str());
                  summaryMLI->value(album.summary.c_str());
         
                  cout << "sending request url: " << album.image << endl;
                  std::ostringstream os;
                  curlpp::Easy myRequest;
                  myRequest.setOpt(new curlpp::options::WriteStream(&os));
                  myRequest.setOpt(new curlpp::options::Url(album.image.c_str()));
                  myRequest.perform();
                  std::string aString = os.str();
                  //std::cout << aString << std::endl;
                  ofstream myfile;
                  myfile.open ("albumCover.png");
                  myfile << aString;
                  myfile.close();
                  png = new Fl_PNG_Image("albumCover.png");
                  box->image(png);
                  //box->set_changed();
                  box->redraw();
               }
               else if(item->depth() == 3){
                  aStr = "Clicked track";
                  Album album;
                  if(library){
                     cout << "trying to get: " << item->label() << endl;
                     Fl_Tree_Item * parent = item->parent();
                     std::string parentTitle(parent->label());
                     Fl_Tree_Item * grandparent = parent->parent();
                     string grandparentTitle(grandparent->label());
                     if(grandparentTitle == "Search Result"){
                        album = searchedAlbum;
                     }
                     else{
                        album = library->get(parentTitle);
                     }
                  }else{
                     cout << "library entry not found" << endl;
                     break;
                  }
                  //album.print();
                  Track track = album.get(aTitle);
                  track.print();
                  trackInput->value(track.track.c_str());
                  string fileName = track.track + ".mp3";
                  fileNameInput->value(fileName.c_str());
                  //fileNameInput->value(track.fileName.c_str());
                  albumInput->value(album.name.c_str());
                  authorInput->value(album.artist.c_str());
                  rankInput->value(track.rank.c_str());
                  genreChoice->clear();
                  vector<string>::iterator it = album.genres.begin();
                  while(it != album.genres.end()){
                     genreChoice->add(it->c_str());
                     it++;
                  }
                  genreChoice->value(0);
                  timeInput->value(track.getTrackTime().c_str());
                  summaryMLI->value(album.summary.c_str());
         
                  cout << "sending request url: " << album.image << endl;
                  std::ostringstream os;
                  curlpp::Easy myRequest;
                  myRequest.setOpt(new curlpp::options::WriteStream(&os));
                  myRequest.setOpt(new curlpp::options::Url(album.image.c_str()));
                  myRequest.perform();
                  std::string aString = os.str();
                  //std::cout << aString << std::endl;
                  ofstream myfile;
                  myfile.open ("albumCover.png");
                  myfile << aString;
                  myfile.close();
                  png = new Fl_PNG_Image("albumCover.png");
                  box->image(png);
                  //box->set_changed();
                  box->redraw();
               }
               break;
            }
            case FL_TREE_REASON_DESELECTED: {aStr = "deselected"; break;}
            default: {break;}
         }
         cout << "Callback reason: " << aStr.c_str() << endl;
      }

      // Static menu callback method
      static void Menu_ClickedS(Fl_Widget*w, void*data) {
         MusicBrowser *o = (MusicBrowser*)data;
         o->Menu_Clicked(); //call the instance callback method
      }

      // Menu selection instance method that has iaccess to instance vars.
      void Menu_Clicked() {
         char picked[80];
         menubar->item_pathname(picked, sizeof(picked)-1);
         string selectPath(picked);
         cout << "Selected Menu Path: " << selectPath << endl;
         int select = genreChoice->value();
         //cout << "Selected genre: " << ((select==0)?"rock":"blues") << endl;
         // Handle menu selections
         if(selectPath.compare("File/Save")==0){
            bool restSave = library->toJsonFile("album.json");
            cout << "Save implemented" << endl;
         }else if(selectPath.compare("File/Restore")==0){
            library->initLibraryFromJsonFile("album.json");
            buildTree();
            cout << "Restore implemented" << endl;
         }else if(selectPath.compare("File/Tree Refresh")==0){
            buildTree();
         }else if(selectPath.compare("File/Exit")==0){
            if(playThread && playThread->joinable()){
               playThread->join();
            }
            exit(0);
         }else if(selectPath.compare("Track/Add")==0){
            // Find item that was clicked
            Fl_Tree_Item *item = (Fl_Tree_Item*)tree->item_clicked();
            if(item->depth() == 3){
               Fl_Tree_Item * parent = item->parent();
               std::string parentTitle(parent->label());
               Fl_Tree_Item * grandparent = parent->parent();
               std::string grandparentTitle(grandparent->label());
               string aTitle = item->label();
               if(grandparentTitle.compare("Search Result")==0){
                  library->addTrack(parentTitle, searchedAlbum.get(aTitle) 
                     , searchedAlbum);
               }
               else{
                  Track track = library->getTrack(parentTitle, aTitle);
                  library->addTrack(parentTitle, track , searchedAlbum);
               }
            }
            buildTree();
            cout << "Add track implemented" << endl;
         }else if(selectPath.compare("Track/Remove")==0){
            // Find item that was clicked
            Fl_Tree_Item *item = (Fl_Tree_Item*)tree->item_clicked();
            if(item->depth() == 3){
               Fl_Tree_Item * parent = item->parent();
               std::string parentTitle(parent->label());
               string aTitle = item->label();
               cout << library->removeTrack(parentTitle, aTitle);
            }
            buildTree();
            cout << "Remove track implemented" << endl;
         }else if(selectPath.compare("Album/Add")==0){
            // Find item that was clicked
            Fl_Tree_Item *item = (Fl_Tree_Item*)tree->item_clicked();
            Fl_Tree_Item *parent = item->parent();
            string parentTitle(parent->label());
            if(item->depth() == 2){
               cout << parentTitle << endl;
               if(parentTitle.compare("Search Result") == 0){
                  library->addAlbum(searchedAlbum);
               }
               else{
                  string albumName = item->label();
                  Album album = library->get(albumName);
                  library->addAlbum(album);
               }
            }
            buildTree();
            cout << "Add album implemented" << endl;
         }else if(selectPath.compare("Album/Remove")==0){
            // Find item that was clicked
            Fl_Tree_Item *item = (Fl_Tree_Item*)tree->item_clicked();
            if(item->depth() == 2){
               cout << library->removeAlbum(item->label());
            }
            buildTree();
            cout << "Remove album implemented" << endl;
         }else if(selectPath.compare("Track/Play")==0){
            std::string unameres = exec("uname");
            std::string pwdPath = exec("pwd");
            pwdPath = pwdPath.substr(0,pwdPath.length()-1);
            std::cout << "OS type is: " << unameres << " curr.dir is: "
               << pwdPath << std::endl;
            // This path is only valid on linux so we will have to check ostype
            std::stringstream streamLinux;
            streamLinux << "/usr/bin/vlc "
               << pwdPath << "/MediaFiles/" << trackInput->value()
               << ".mp3" ;
            std::string aStr("Linux");
            std::stringstream streamMac;
            streamMac << "/Applications/VLC.app/Contents/MacOS/VLC "
               << pwdPath << "/MediaFiles/" << trackInput->value()
               << ".mp3" ;
            cout << "mac command: " << streamMac.str() << endl;
            cout << "linux command: " << streamLinux.str() << endl;
            // start vlc to play the media file
            //limit the comparison to the length of Linux to remove new line char
            if(unameres.compare(0,aStr.length(),aStr)==0){
               string argLinux(streamLinux.str());
               cmd = argLinux;
               playThread = new std::thread(run);
            }else{
               string arg(streamMac.str());
               cmd = arg;
               playThread = new std::thread(run);
            }
         }
      }

      /**
       * a static method to remove spaces, tabs, new lines and returns from the
       * begining or end of a string.
       */
      static std::string& trimMe (std::string& str) {
         // right trim
         while (str.length() > 0 && (str[str.length()-1] == ' '  ||
                  str[str.length()-1] == '\t' ||
                  str[str.length()-1] == '\n' ||
                  str[str.length()-1] == '\r')){
            str.erase(str.length ()-1, 1);
         }
         // left trim
         while (str.length () > 0 && (str[0] == ' ' || str[0] == '\t')){
            str.erase(0, 1);
         }
         return str;
      }

      /**
       * a method to execute a command line command and to return
       * the resulting string.
       */
      std::string exec(const char* cmd) {
         FILE* pipe = popen(cmd, "r");
         if (!pipe) return "ERROR";
         char buffer[128];
         std::string result = "";
         while(!feof(pipe)) {
            if(fgets(buffer, 128, pipe) != NULL)
               result += buffer;
         }
         pclose(pipe);
         return result;
      }

      void buildTree(){
         vector<string> result = library->getAlbums();
         //cout << "server has titles";
         tree->clear();
         for(int i=0; i<result.size(); i++){
            //cout << " " << result[i];
            Album album = library->get(result[i]);
            //cout << album.name << " " << md.album << " " << md.author
            //     << " " << md.genre << endl;
            //album.print();
            std::map<string,Track>::iterator it = album.tracks.begin();
            while(it != album.tracks.end()){
               std::stringstream stream;
               stream << "Your Music"
                  << "/"
                  << album.name
                  << "/" << it->second.track;
               tree->add(stream.str().c_str());
               it++;
               //stream.str("");
            }
         }
         //searchedAlbum.print();
         map<string,Track>::iterator it = searchedAlbum.tracks.begin();
         while(it != searchedAlbum.tracks.end()){
            stringstream stream;
            stream << "Search Result"
               << "/"
               << searchedAlbum.name
               << "/" << it->second.track;
            tree->add(stream.str().c_str());
            it++;
            //stream.str("");
         }
         cout << "end of buildTree" << endl;
         //tree->root()->label("Music Browser");
         tree->redraw();
      }

      MusicBrowser(const char * name = "Marcus", const char * key = "myKey") : MediaClientGui(name) {
         searchButt->callback(SearchCallbackS, (void*)this);
         menubar->callback(Menu_ClickedS, (void*)this);
         tree->callback(TreeCallbackS, (void*)this);
         callback(ClickedX);
         lastfmkey = key;
         userId = "Marcus.Miller";
         library = new MusicLibrary();
         buildTree();
      }
};

int main(int argc, char * argv[]) {
   std::string developer = (argc>1)?argv[1]:"Marcus.Miller";
   std::string lastfmkey = (argc>2)?argv[2]:"lastfmkey";
   std::string windowTitle = developer + "'s Music Browser";
   MusicBrowser cm(windowTitle.c_str(),lastfmkey.c_str());
   return (Fl::run());
}
