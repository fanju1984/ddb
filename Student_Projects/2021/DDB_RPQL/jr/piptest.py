            tasks = []
            with ThreadPoolExecutor(max_workers=5) as t:
                for i in range(len(fragment)):
                    subdata = data[i]
                    len_of_fragment = len(subdata)
                    # reverse_sitenames = {v:k for k,v in sitenames.items()}
                    
                    etcd.put("/table/{}/lenfragment/{}".format(table_name, fragment[i]["sitename"]), str(len_of_fragment))
                    bytedata = pickle.dumps(subdata)
                    ip, port, db = fragment[i]["site"].split(":")
                    print("start", i)
                    # tasks.append(tmp(i, ip, port, table_name, fragment_columns, bytedata))
                    task = t.submit(tmp, i, ip, port, table_name, fragment_columns, bytedata)
                    tasks.append(task)
                
                for future in as_completed(tasks):
                    message, ip, port = future.result()
                    if ip+":"+port in namesites:
                        sitename = namesites[ip+":"+port]
                        print(f"message: {message} on {sitename}", time.asctime())
                    else:
                        print(f"message: {message} on {ip}:{port}", time.asctime())