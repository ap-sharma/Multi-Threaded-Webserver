#include "http_server.hh"

#include <vector>

#include <sys/stat.h>

#include <fstream>
#include <sstream>



vector<string> split(const string &s, char delim) {
  vector<string> elems;

  stringstream ss(s);                                   // Converting string s to stringstream object ss
  string item;

  while (getline(ss, item, delim)) {
    if (!item.empty())
      elems.push_back(item);
  }

  return elems;
}

HTTP_Request::HTTP_Request(string request) {
  vector<string> lines = split(request, '\n');  
  vector<string> first_line = split(lines[0], ' ');
  //cout<<"printing lines value:" << lines[0] << endl;
  
  /*extract the request method and URL from first_line here
  */
  this->HTTP_version = "1.0"; // We'll be using 1.0 irrespective of the request
  this->method =first_line.at(0);
  this->url = first_line.at(1);
  
   //cout << "Element at Index 0: " << first_line.at(0) << endl;


  if (this->method != "GET") {
    cerr << "Method '" << this->method << "' not supported" << endl;
    exit(1);
  }
}

HTTP_Response *handle_request(string req) {
  HTTP_Request *request = new HTTP_Request(req);

  HTTP_Response *response = new HTTP_Response();

  string url = string("html_files") + request->url;

  response->HTTP_version = "1.0";

  struct stat sb;
  if (stat(url.c_str(), &sb) == 0) // requested path exists
  {
    //printf("requested path exists \n");
    response->status_code = "200";
    response->status_text = "OK";
    response->content_type = "text/html";

    string body;
    body.clear();

    if (S_ISDIR(sb.st_mode)) {
      /*
      In this case, requested path is a directory.
      TODO : find the index.html file in that directory (modify the url
      accordingly)
      */
     url=url+string("/index.html");
    }

    /*
    TODO : open the file and read its contents
    */
    fstream my_file;
    string st;
    my_file.open(url,ios::in);
    int byte_count;
    
      if(!my_file){
            cout << "No such file";
      }
      else {
          char ch;
          
          while (1) {
            //my_file >> ch;
            
            if (my_file.eof())
              {//cout<<body;
              break;}
            //cout << ch;
            getline(my_file,st);
            body=body + st +"\n";
            byte_count++;
          }
          }
          my_file.close();
        
    /*
    TODO : set the remaining fields of response appropriately
    */
   response->content_length = to_string(long(sb.st_size));
   //cout<<"\n sb.st_size is \n"<<sb.st_size<<endl;
   response->body = body;
   body.clear();
  }

  else {
    
    //printf("requested path does not exist \n");
    string body;
    url.clear();
    url=string("error.html");
    stat(url.c_str(), &sb);
    response->status_code = "404";

    /*
    TODO : set the remaining fields of response appropriately
    */
    response->status_text = "NOT FOUND";
    response->content_type = "text/html";
    fstream my_file_else;
    my_file_else.open(url,ios::in);
    int byte_count;
    
    if(!my_file_else){
          cout << "No such file";
    }
    else {
        char ch;
        string st;
        
        while (1) {
          //my_file_else >> ch;
          if (my_file_else.eof())
            {//cout <<body;              
              break;}
          //cout << ch;
          getline(my_file_else,st);
          body=body + st+"\n";
          byte_count++;
        }
        }
    my_file_else.close();
    response->content_length = to_string(long(sb.st_size));
    //cout<<"\n sb.st_size is \n"<<sb.st_size<<endl;
    response->body = body;
    //cout<<body<<endl;
    body.clear();
  }

  delete request;

  return response;
}

string HTTP_Response::get_string() {
  /*
  TODO : implement this function
  */
 string output="HTTP/"+this->HTTP_version+" "+this->status_code+" "+this->status_text+"\r\n"+"content_type:"+this->content_type+"\r\n"+"content_length:"+this->content_length+"\r\n\r\n\r\n"+this->body; 

 return output;
}
