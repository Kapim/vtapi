
#include <vtapi/common/global.h>
#include <vtapi/common/exception.h>
#include <vtapi/common/defs.h>
#include <vtapi/data/taskprogress.h>

using namespace std;

namespace vtapi {


TaskProgress::TaskProgress(const Commons &commons,
                           const string& taskname,
                           const string& seqname)
    : KeyValues(commons)
{
    if (context().dataset.empty())
        throw BadConfigurationException("dataset not specified");

    if (!taskname.empty())
        context().task = taskname;

    if (!seqname.empty())
        context().sequence = seqname;

    _select.from(def_tab_tasks_seq, def_col_all);

    if(!context().task.empty())
        _select.whereString(def_col_tsd_taskname, context().task);

    if (!context().sequence.empty())
        _select.whereString(def_col_tsd_seqname, context().sequence);
}


TaskProgress::TaskProgress(const Commons &commons,
                           const string& taskname,
                           const list<string> &seqnames)
    : KeyValues(commons)
{
    if (context().dataset.empty())
        throw BadConfigurationException("dataset not specified");

    if (!taskname.empty())
        context().task = taskname;

    _select.from(def_tab_tasks_seq, def_col_all);

    if(!context().task.empty())
        _select.whereString(def_col_tsd_taskname, context().task);

    _select.whereStringInList(def_col_tsd_seqname, seqnames);
}

bool TaskProgress::next()
{
    if (KeyValues::next()) {
        context().task = this->getTaskName();
        context().sequence = this->getSequenceName();
        return true;
    }
    else {
        return false;
    }
}

string TaskProgress::getTaskName()
{
    return this->getString(def_col_tsd_taskname);
}

Task *TaskProgress::getTask()
{
    return new Task(*this, this->getTaskName());
}

int TaskProgress::getProcessId()
{
    return this->getInt(def_col_tsd_prsid);
}


Process *TaskProgress::getProcess()
{
    return new Process(*this, this->getProcessId());
}

string TaskProgress::getSequenceName()
{
    return this->getString(def_col_tsd_seqname);
}


Sequence *TaskProgress::getSequence()
{
    return new Sequence(*this, this->getSequenceName());
}

bool TaskProgress::getIsDone()
{
    return this->getBool(def_col_tsd_isdone);
}

time_t TaskProgress::getStartedTime()
{
    return this->getTimestamp(def_col_tsd_started);
}


time_t TaskProgress::getFinishedTime()
{
    return this->getTimestamp(def_col_tsd_finished);
}

bool TaskProgress::updateIsDone(bool value)
{
    time_t t;
    std::time(&t);

    return this->updateBool(def_col_tsd_isdone, value) &&
            this->updateTimestamp(def_col_tsd_finished, t) &&
            this->updateExecute();
}

bool TaskProgress::preUpdate()
{
    bool ret = KeyValues::preUpdate(def_tab_tasks_seq);
    if (ret) {
        ret &= _update->whereString(def_col_tsd_taskname, context().task);
        ret &= _update->whereString(def_col_tsd_seqname, context().sequence);
    }

    return ret;
}

}