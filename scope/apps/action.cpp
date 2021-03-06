/*
 * Copyright (C) 2016 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * * Authored by:
 *	Kyle Nitzsche <kyle.nitzsche@canonical.com>
 */

#include "scope/apps/action.h"
#include "scope/apps/config.h"
#include "scope/apps/hidden_apps.h"
#include <unity/scopes/ActivationResponse.h>
#include <unity/scopes/CannedQuery.h>
#include <url-dispatcher.h>
#include <QString>
#include <QFile>
#include <QTextStream>

namespace usc = unity::scopes;


namespace
{
static usc::ActivationResponse
sendToResults(usc::FilterState const& filter_state)
{
  usc::CannedQuery query(FULLY_QUALIFIED_APPS_SCOPE);
  query.set_filter_state(filter_state);
  return usc::ActivationResponse(query);
}
}


Action::
Action(unity::scopes::Result const&         result,
       unity::scopes::ActionMetadata const& metadata,
       std::string const&                   action_id,
       Action::OpenUriAction                open_action,
       std::shared_ptr<HiddenApps>          hidden,
       unity::scopes::FilterState const&    filter_state)
  : usc::ActivationQueryBase(result, metadata)
  , action_id_(action_id)
  , open_action_(open_action)
  , hidden_(hidden)
  , filter_state_(filter_state)
{
}


usc::ActivationResponse
Action::activate()
{
  if (action_id_ == "open")
  {
    open_action_(result().uri());
  }
  else if (action_id_ == "hide")
  {
    hidden_->add(QString::fromStdString(result()["app_id"].get_string()));
    return sendToResults(filter_state_);
  }
  else if (action_id_ == "show")
  {
    hidden_->remove(QString::fromStdString(result()["app_id"].get_string()));
    return sendToResults(filter_state_);
  }
  return usc::ActivationResponse(usc::ActivationResponse::Status::NotHandled);
}
