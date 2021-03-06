/* -------------------------------------------------------------------------- */
/* Copyright 2002-2019, OpenNebula Project, OpenNebula Systems                */
/*                                                                            */
/* Licensed under the Apache License, Version 2.0 (the "License"); you may    */
/* not use this file except in compliance with the License. You may obtain    */
/* a copy of the License at                                                   */
/*                                                                            */
/* http://www.apache.org/licenses/LICENSE-2.0                                 */
/*                                                                            */
/* Unless required by applicable law or agreed to in writing, software        */
/* distributed under the License is distributed on an "AS IS" BASIS,          */
/* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.   */
/* See the License for the specific language governing permissions and        */
/* limitations under the License.                                             */
/* -------------------------------------------------------------------------- */

#ifndef REQUEST_MANAGER_RENAME_H_
#define REQUEST_MANAGER_RENAME_H_

#include "Request.h"
#include "Nebula.h"

using namespace std;

/* ------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------- */

class RequestManagerRename : public Request
{
protected:
    RequestManagerRename(const string& method_name,
                         const string& help,
                         const string& params = "A:sis")
        :Request(method_name,params,help)
    {
        pthread_mutex_init(&mutex, 0);

        auth_op = AuthRequest::MANAGE;
    };

    virtual ~RequestManagerRename(){};

    /* -------------------------------------------------------------------- */

    void request_execute(xmlrpc_c::paramList const& _paramList,
                        RequestAttributes& att);

    /**
     *  Gets and object by name and owner. Default implementation returns no
     *  object
     */
    virtual int exist(const string& name, int uid) = 0;

    /**
     *  Batch rename of related objects. Default implementation does nothing
     */
    virtual void batch_rename(int oid){};

    /**
     *  Test if a rename is being perform on a given object. If not it set it.
     *    @return true if the rename can be performed (no ongoing rename)
     */
    bool test_and_set_rename(int oid)
    {
        pair<set<int>::iterator,bool> rc;

        pthread_mutex_lock(&mutex);

        rc = rename_ids.insert(oid);

        pthread_mutex_unlock(&mutex);

        return rc.second == true;
    }

    /**
     *  Clear the rename.
     */
    void clear_rename(int oid)
    {
        pthread_mutex_lock(&mutex);

        rename_ids.erase(oid);

        pthread_mutex_unlock(&mutex);
    }

private:
    /**
     *  Mutex to control concurrent access to the ongoing rename operations
     */
    pthread_mutex_t mutex;

    /**
     *  Set of IDs being renamed;
     */
    set<int> rename_ids;

};

/* ------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------- */

class VirtualMachineRename : public RequestManagerRename
{
public:
    VirtualMachineRename():
        RequestManagerRename("one.vm.rename","Renames a virtual machine")
    {
        Nebula& nd  = Nebula::instance();
        pool        = nd.get_vmpool();
        auth_object = PoolObjectSQL::VM;

        auth_op     = nd.get_vm_auth_op(History::RENAME_ACTION);
    };

    ~VirtualMachineRename(){};

    int exist(const string& name, int uid)
    {
        return -1;
    }
};

/* ------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------- */

class TemplateRename : public RequestManagerRename
{
public:
    TemplateRename():
        RequestManagerRename("one.template.rename",
                             "Renames a virtual machine template")
    {
        Nebula& nd  = Nebula::instance();
        pool        = nd.get_tpool();
        auth_object = PoolObjectSQL::TEMPLATE;
    };

    ~TemplateRename(){};

    int exist(const string& name, int uid)
    {
        return pool->exist(name, uid);
    }
};

/* ------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------- */

class VirtualNetworkTemplateRename : public RequestManagerRename
{
public:
    VirtualNetworkTemplateRename():
        RequestManagerRename("one.vntemplate.rename",
                             "Renames a virtual network template")
    {
        Nebula& nd  = Nebula::instance();
        pool        = nd.get_vntpool();
        auth_object = PoolObjectSQL::VNTEMPLATE;
    };

    ~VirtualNetworkTemplateRename(){};

    int exist(const string& name, int uid)
    {
        return pool->exist(name, uid);
    }
};

/* ------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------- */


class VirtualNetworkRename: public RequestManagerRename
{
public:
    VirtualNetworkRename():
        RequestManagerRename("one.vn.rename","Renames a virtual network")
    {
        Nebula& nd  = Nebula::instance();
        pool        = nd.get_vnpool();
        auth_object = PoolObjectSQL::NET;
    };

    ~VirtualNetworkRename(){};

    int exist(const string& name, int uid)
    {
        return pool->exist(name, uid);
    }
};

/* ------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------- */

class ImageRename: public RequestManagerRename
{
public:
    ImageRename():
        RequestManagerRename("one.image.rename", "Renames an image")
    {
        Nebula& nd  = Nebula::instance();
        pool        = nd.get_ipool();
        auth_object = PoolObjectSQL::IMAGE;
    };

    ~ImageRename(){};

    int exist(const string& name, int uid)
    {
        return pool->exist(name, uid);
    }
};

/* ------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------- */

class DocumentRename : public RequestManagerRename
{
public:
    DocumentRename():
        RequestManagerRename("one.document.rename", "Renames a generic document")
    {
        Nebula& nd  = Nebula::instance();
        pool        = nd.get_docpool();
        auth_object = PoolObjectSQL::DOCUMENT;
    };

    ~DocumentRename(){};

    int exist(const string& name, int uid)
    {
        return -1;
    }
};

/* ------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------- */

class ClusterRename: public RequestManagerRename
{
public:
    ClusterRename():
        RequestManagerRename("one.cluster.rename", "Renames a cluster")
    {
        Nebula& nd  = Nebula::instance();
        pool        = nd.get_clpool();
        auth_object = PoolObjectSQL::CLUSTER;
    };

    ~ClusterRename(){};

    int exist(const string& name, int uid)
    {
        return pool->exist(name);
    }

    void batch_rename(int oid);
};

/* ------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------- */

class DatastoreRename: public RequestManagerRename
{
public:
    DatastoreRename():
        RequestManagerRename("one.datastore.rename", "Renames a datastore")
    {
        Nebula& nd  = Nebula::instance();
        pool        = nd.get_dspool();
        auth_object = PoolObjectSQL::DATASTORE;
    };

    ~DatastoreRename(){};

    int exist(const string& name, int uid)
    {
        return pool->exist(name);
    }

    void batch_rename(int oid);
};

/* ------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------- */

class HostRename: public RequestManagerRename
{
public:
    HostRename():
        RequestManagerRename("one.host.rename", "Renames a host")
    {
        Nebula& nd  = Nebula::instance();
        pool        = nd.get_hpool();
        auth_object = PoolObjectSQL::HOST;

        auth_op = AuthRequest::ADMIN;
    };

    ~HostRename(){};

    int exist(const string& name, int uid)
    {
        return pool->exist(name);
    }

    void batch_rename(int oid);
};

/* ------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------- */

class ZoneRename : public RequestManagerRename
{
public:
    ZoneRename():
        RequestManagerRename("one.zone.rename", "Renames a zone")
    {
        Nebula& nd  = Nebula::instance();
        pool        = nd.get_zonepool();
        auth_object = PoolObjectSQL::ZONE;
    };

    ~ZoneRename(){};

    int exist(const string& name, int uid)
    {
        return pool->exist(name);
    }
};

/* ------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------- */

class SecurityGroupRename: public RequestManagerRename
{
public:
    SecurityGroupRename():
        RequestManagerRename("one.secgroup.rename", "Renames a security group")
    {
        Nebula& nd  = Nebula::instance();
        pool        = nd.get_secgrouppool();
        auth_object = PoolObjectSQL::SECGROUP;
    };

    ~SecurityGroupRename(){};

    int exist(const string& name, int uid)
    {
        return pool->exist(name, uid);
    }
};

/* ------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------- */

class VdcRename : public RequestManagerRename
{
public:
    VdcRename():
        RequestManagerRename("one.vdc.rename", "Renames a VDC")
    {
        Nebula& nd  = Nebula::instance();
        pool        = nd.get_vdcpool();
        auth_object = PoolObjectSQL::VDC;
    };

    ~VdcRename(){};

    int exist(const string& name, int uid)
    {
        return pool->exist(name);
    }
};

/* ------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------- */

class VirtualRouterRename: public RequestManagerRename
{
public:
    VirtualRouterRename():
        RequestManagerRename("one.vrouter.rename", "Renames a virtual router")
    {
        Nebula& nd  = Nebula::instance();
        pool        = nd.get_vrouterpool();
        auth_object = PoolObjectSQL::VROUTER;
    };

    ~VirtualRouterRename(){};

    int exist(const string& name, int uid)
    {
        return -1;
    }
};

/* ------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------- */

class MarketPlaceRename: public RequestManagerRename
{
public:
    MarketPlaceRename():
        RequestManagerRename("one.market.rename", "Renames a marketplace")
    {
        Nebula& nd  = Nebula::instance();
        pool        = nd.get_marketpool();
        auth_object = PoolObjectSQL::MARKETPLACE;
    };

    ~MarketPlaceRename(){};

    int exist(const string& name, int uid)
    {
        return pool->exist(name);
    }

    void batch_rename(int oid);
};

/* ------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------- */

class MarketPlaceAppRename: public RequestManagerRename
{
public:
    MarketPlaceAppRename():
        RequestManagerRename("one.marketapp.rename", "Renames a marketplace app")
    {
        Nebula& nd  = Nebula::instance();
        pool        = nd.get_apppool();
        auth_object = PoolObjectSQL::MARKETPLACEAPP;
    };

    ~MarketPlaceAppRename(){};

    int exist(const string& name, int uid)
    {
        return pool->exist(name, uid);
    }
};

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

class VMGroupRename: public RequestManagerRename
{
public:
    VMGroupRename():
        RequestManagerRename("one.vmgroup.rename", "Renames a vm group")
    {
        Nebula& nd  = Nebula::instance();
        pool        = nd.get_vmgrouppool();
        auth_object = PoolObjectSQL::VMGROUP;
    };

    ~VMGroupRename(){};

    int exist(const string& name, int uid)
    {
        return pool->exist(name, uid);
    }
};

#endif
