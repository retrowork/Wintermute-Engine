// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt
// http://dead-code.org/redir.php?target=wme

using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing.Design;
using System.ComponentModel;
using System.Windows.Forms;
using DeadCode.WME.Core;
using System.Media;

namespace DeadCode.WME.Global.UITypeEditors
{
	public class EntityTypeEditor : UITypeEditor
	{

		//////////////////////////////////////////////////////////////////////////
		public override UITypeEditorEditStyle GetEditStyle(ITypeDescriptorContext context)
		{
			return UITypeEditorEditStyle.Modal;
		}

		//////////////////////////////////////////////////////////////////////////
		public override object EditValue(ITypeDescriptorContext context, IServiceProvider provider, object value)
		{
			if (context == null) return value;
			NativePropProxy Proxy = context.Instance as NativePropProxy;
			if (Proxy == null || Proxy.NativeObject == null)
			{
				SystemSounds.Asterisk.Play();
				return value;
			}

			WAdEntity OrigObj = value as WAdEntity;

			FileAssignForm dlg = new FileAssignForm();
			dlg.Game = Proxy.NativeObject.Game;
			dlg.ParentForm = Form.ActiveForm;
			dlg.Filter = "Entities|*.entity";
			if (OrigObj != null && OrigObj.Filename != null)
				dlg.Filename = OrigObj.Filename;

			DialogResult Res = dlg.ShowDialog();

			switch (Res)
			{
				case DialogResult.OK:
					try
					{
						WAdEntity NewObj = new WAdEntity(Proxy.NativeObject.Game);
						if (!NewObj.LoadFromFile(dlg.Filename))
						{
							NewObj.Dispose();
							return value;
						}
						else return NewObj;
					}
					catch
					{
						return value;
					}
				case DialogResult.Abort:
					return null;
				default:
					return value;
			}

		}
	}
}
