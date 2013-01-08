/*
 * NullCompletionManager.java
 *
 * Copyright (C) 2009-12 by RStudio, Inc.
 *
 * Unless you have received this program directly from RStudio pursuant
 * to the terms of a commercial license agreement with RStudio, then
 * this program is licensed to you under the terms of version 3 of the
 * GNU Affero General Public License. This program is distributed WITHOUT
 * ANY EXPRESS OR IMPLIED WARRANTY, INCLUDING THOSE OF NON-INFRINGEMENT,
 * MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE. Please refer to the
 * AGPL (http://www.gnu.org/licenses/agpl-3.0.txt) for more details.
 *
 */
package org.rstudio.studio.client.workbench.views.console.shell.assist;

import com.google.gwt.dom.client.NativeEvent;

public class NullCompletionManager implements CompletionManager
{
   public void close()
   {
   }
   
   public void goToHelp()
   {
   }
   
   public void goToFunctionDefinition()
   {
   }
   
   public void codeCompletion()
   {
   }

   public boolean previewKeyDown(NativeEvent event)
   {
      return false;
   }

   public boolean previewKeyPress(char charCode)
   {
      return false;
   }
}
