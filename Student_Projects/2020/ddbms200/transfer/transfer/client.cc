#include "transfer.h"

WEB site_info[4] = {{"10.46.221.72","50051","2379"},
                    {"10.46.143.129","50051","2379"},
                    {"10.46.33.43","50051","2379"},
                    {"10.46.33.43","50052","22379"}
                };

string TransferClient::local_I_D(string sql, int site){
    Stmt1 stmt;
    stmt.set_sql(sql);
    stmt.set_site(site);

    Reply re;

    ClientContext context;

    Status status = stub_->L_I_D(&context, stmt, &re);
    if (status.ok()) {
      return re.done();
    } else {
      cout << status.error_code() << ": " << status.error_message() << endl;
      return re.done();
    }
}
string TransferClient::local_L(string sql1,string sql2, int site){
    Stmt2 stmt;
    stmt.set_sql1(sql1);
    stmt.set_sql2(sql2);
    stmt.set_site(site);

    Reply re;

    ClientContext context;

    Status status = stub_->L_L(&context, stmt, &re);
    if (status.ok()) {
      return re.done();
    } else {
      cout << status.error_code() << ": " << status.error_message() << endl;
      return re.done();;
    }
}
string TransferClient::local_S(string sql,string file_name, int site){
    Stmt2 stmt;
    stmt.set_sql1(sql);
    stmt.set_sql2(file_name);
    stmt.set_site(site);
    // 开始接收返回，并写入文件
    ClientContext context;
    Chunk chunk;
    string fl = TMPPATH + file_name+".sql";
    const char *filename = fl.c_str();
    ofstream outfile;
    const char *data;
    outfile.open(filename, ofstream::out | ofstream::trunc | ofstream::binary);
    std::unique_ptr<ClientReader<Chunk> > reader(
        stub_->L_S(&context, stmt));
    while (reader->Read(&chunk)) {
        data = chunk.buffer().c_str();
        outfile.write(data, chunk.buffer().length());
    }
    long pos = outfile.tellp();
    cout << "length:    " << pos << endl;
    outfile.close();

    Status status = reader->Finish();
    
    if (status.ok()) {
      return file_name;
    } else {
      cout << site << ":  " << site << " has something wrong! Please check it!" << endl;
      cout << status.error_code() << ": " << status.error_message() << endl;
      return "";
    }
}
string TransferClient::local_T_L(string tmp_data, int site){
    TMPFile fi;
    Stmt1 st;
    st.set_sql(tmp_data);
    st.set_site(site);
    fi.mutable_cfg()->CopyFrom(st);

    //读取文件，传输给server    
    string fl = TMPPATH + tmp_data +".sql";
    const char *filename = fl.c_str();

    Chunk chunk;
    char data[CHUNK_SIZE];
    Reply re;
    ClientContext context;

    ifstream infile;
    int len = 0;
    struct timeval start, end;
    gettimeofday(&start, NULL);

    infile.open(filename, ifstream::in | ifstream::binary);
    unique_ptr<ClientWriter<TMPFile>> writer(stub_->L_T_L(&context, &re));
    while (!infile.eof()) {
      infile.read(data, CHUNK_SIZE);
      chunk.set_buffer(data, infile.gcount());
      fi.mutable_chk()->CopyFrom(chunk);
      if (!writer->Write(fi)) {
        // Broken stream.
        break;
      }
      len += infile.gcount();
    }
    writer->WritesDone();
    Status status = writer->Finish();
    gettimeofday(&end, NULL);
    cout <<  (end.tv_sec-start.tv_sec)+ (double)(end.tv_usec-start.tv_usec)/1000000 << endl;
    // string res = re.done();
    // return res;
    if (status.ok()) {
      return "OK";
    } else {
      cout << "site:  " << site << "   has something wrong! Please check it!" << endl;
      cout << status.error_code() << ": " << status.error_message() << endl;
      return "";
    }
}
exec_tree TransferClient::Data_S_E(TREE tree, int site){
  MTree mtree;
  mtree.set_treeid(tree.tree_id);
  mtree.set_root(tree.root);
  for(int i=0;i<tree.Nodes.size();i++){
    MNode* mnode;
    mnode = mtree.add_nodes();
    NODE tmp = tree.Nodes[i];
    mnode->set_id(tmp.id);
    for(int j=0;j<tmp.child.size();j++){
      mnode->add_child(tmp.child[j]);
    }
    mnode->set_parent(tmp.parent);
    mnode->set_sqlstmt(tmp.sql_statement);
    mnode->set_site(tmp.site);
  }
  ETree etree;

  ClientContext context;
  Status status = stub_->D_S_E(&context, mtree, &etree);
  if (status.ok()) {
    // 将etree转为exec_tree返回
    exec_tree res;
    res.tree_id = etree.treeid();
    res.root = etree.root();
    for(int i=0;i<etree.nodes_size();i++){
      ENode eno = etree.nodes(i);
      exec_node exeno;
      exeno.node_id = eno.nodeid();
      exeno.time_spend = eno.timespend();
      exeno.volume = eno.volume();
      exeno.res = eno.res();
      for(int j=0;j<eno.child_size();j++){
        exeno.child.push_back(eno.child(j));
      }
      // exeno.child = eno.child();
      exeno.parent = eno.parent();
      exeno.site = eno.site();
      res.Nodes.push_back(exeno);
    }
    return res;
  } else {
    cout << status.error_code() << ": " << status.error_message() << endl;
  } 
}

// RPC_  是executor调用的接口定义
string RPC_local_Insert_Delete(string sql, string site){
    site = site[1];
    char *p = (char*)site.c_str();
    int st = atoi(p);
    TransferClient t1(grpc::CreateChannel(site_info[st-1].IP+":"+site_info[st-1].RPC_PORT, grpc::InsecureChannelCredentials()));
    string res = t1.local_I_D(sql,st);
    return res;
}
string RPC_local_Load(string sql_create, string sql_load, string site){
    site = site[1];
    char *p = (char*)site.c_str();
    int st = atoi(p);
    TransferClient t1(grpc::CreateChannel(site_info[st-1].IP+":"+site_info[st-1].RPC_PORT, grpc::InsecureChannelCredentials()));
    string res = t1.local_L(sql_create,sql_load,st);
    return res;

}
string RPC_Local_Select(string sql, string res_name, string site){
    site = site[1];
    char *p = (char*)site.c_str();
    int st = atoi(p);
    TransferClient t1(grpc::CreateChannel(site_info[st-1].IP+":"+site_info[st-1].RPC_PORT, grpc::InsecureChannelCredentials()));
    string res = t1.local_S(sql,res_name,st);
    return res_name;
}
string RPC_Local_Tmp_Load(string tmp_data, string site){
    site = site[1];
    char *p = (char*)site.c_str();
    int st = atoi(p);
    TransferClient t1(grpc::CreateChannel(site_info[st-1].IP+":"+site_info[st-1].RPC_PORT, grpc::InsecureChannelCredentials()));
    string res = t1.local_T_L(tmp_data,st);
    return res;
}
exec_tree RPC_Data_Select_Execute(TREE tree, string site){
    site = site[1];
    char *p = (char*)site.c_str();
    int st = atoi(p);
    TransferClient t1(grpc::CreateChannel(site_info[st-1].IP+":"+site_info[st-1].RPC_PORT, grpc::InsecureChannelCredentials()));
    return t1.Data_S_E(tree,st);
}
string RPC_GET_FILE(string filename, string site){
  string sql = "";
  string res = RPC_Local_Select(sql, filename, site);
  return "OK";
}
// int main(){
//     // string res = RPC_local_Insert_Delete("create table test2(id int(6))","s2");
//     // string res = RPC_Local_Select("","test","3");
//     string res = RPC_Local_Tmp_Load("test","s2");

//     cout << "res is : " << res << endl;
//     return 0;
// }